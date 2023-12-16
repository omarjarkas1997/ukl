# Unikernel Linux
# Copyright (C) 2018-2022 Red Hat Inc., Boston University,
# Ali Raza, Tommy Unger, Eric Munson, Richard W.M. Jones.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

EXTRA_DIST = \
	.gitignore \
	glibc-build/.gitignore \
	libgcc-build/.gitignore \
	LICENSE \
	README.md

CLEANFILES = \
	*~ *.o *.a \
	stamp-gcc-prereq \
	stamp-h1 \
	stamp-glibc-configure \
	stamp-libgcc-build \
	stamp-libgcc-configure \
	ukl-initrd.cpio.xz \
	vmlinuz

clean-local:
	-rm -rf glibc-build/* libgcc-build/*
	-cd linux && $(MAKE) clean
	-cd $(PROGRAM) && $(MAKE) clean
	-cd initrd && $(MAKE) clean
	-find -name config.cache -delete

min-clean:
	-cd linux && $(MAKE) clean
	-rm UKL.a
	-cd $(PROGRAM) && $(MAKE) clean

app-clean:
	-rm UKL.a
	-cd $(PROGRAM) && $(MAKE) clean

kern-clean:
	-rm vmlinuz
	-cd linux && $(MAKE) clean

DIST_SUBDIRS = dnsmasq hello seed memcached redis mpc-bench fio

#----------------------------------------------------------------------
# Build libgcc and static libstdc++.
#
# XXX We only want libgcc.  Can we build this in a way which doesn't
# build the whole of gcc?

# C runtime objects.
CRT_OBJS = crtbeginT.o crtend.o libgcc.a libgcc_eh.a

noinst_DATA = $(CRT_OBJS) libstdc++.a

GCC_CFLAGS = -ggdb -O2 -mcmodel=kernel -mno-red-zone
if DEFAULT_PIE
GCC_CFLAGS += -fno-PIC -no-pie -fno-PIE
endif

$(CRT_OBJS): stamp-libgcc-build
stamp-libgcc-build: stamp-libgcc-configure
	rm -f $@
	cd libgcc-build && $(MAKE) all-gcc
	-cd libgcc-build && $(MAKE) all-target-libgcc \
	    CFLAGS_FOR_TARGET='$(GCC_CFLAGS)' \
	    CXXFLAGS_FOR_TARGET='$(GCC_CFLAGS)'
# XXX The kernel memory model doesn't support -fpic, but GCC adds it
# unconditionally, even though we used --disable-shared.
	$(SED) -i 's/PICFLAG/#PICFLAG/g' \
	    libgcc-build/x86_64-pc-linux-gnu/libgcc/Makefile
	cd libgcc-build && $(MAKE) all-target-libgcc \
	    CFLAGS_FOR_TARGET='$(GCC_CFLAGS)' \
	    CXXFLAGS_FOR_TARGET='$(GCC_CFLAGS)'
# libgcc_eh.a is not built automatically when we use --disable-shared,
# but it still builds fine.
	cd libgcc-build/x86_64-pc-linux-gnu/libgcc && \
	    $(MAKE) libgcc_eh.a \
	    CFLAGS_FOR_TARGET='$(GCC_CFLAGS)' \
	    CXXFLAGS_FOR_TARGET='$(GCC_CFLAGS)'
	cd libgcc-build/x86_64-pc-linux-gnu/libgcc && \
	cp $(CRT_OBJS) $(abs_builddir)
	touch $@

libstdc++.a: stamp-libgcc-build
	rm -f $@
	cd libgcc-build && $(MAKE) all-target-libstdc++-v3 \
	    CFLAGS_FOR_TARGET='$(GCC_CFLAGS)' \
	    CXXFLAGS_FOR_TARGET='$(GCC_CFLAGS)'
	cp libgcc-build/x86_64-pc-linux-gnu/libstdc++-v3/src/.libs/libstdc++.a $@

stamp-libgcc-configure: stamp-gcc-prereq
	rm -f $@
	mkdir -p libgcc-build
	cd libgcc-build && \
	$(abs_top_srcdir)/gcc/configure \
	    --prefix=$(abs_builddir) \
	    --enable-languages=c,c++ \
	    --disable-nls \
	    --without-headers \
	    --with-multilib-list=m64 \
	    --disable-multilib \
	    --disable-shared
	touch $@

stamp-gcc-prereq:
	rm -f $@
	cd $(srcdir)/gcc && contrib/download_prerequisites
	touch $@

#----------------------------------------------------------------------
# Build glibc.

LIBC_OBJS = libc.a libpthread.a libm.a librt.a libcrypt.a crt1.o crti.o crtn.o

noinst_DATA += $(LIBC_OBJS)

GLIBC_CFLAGS = -g -O2 -fthread-jumps -mcmodel=kernel -mno-red-zone
if DEFAULT_PIE
GLIBC_CFLAGS += -fno-PIC -no-pie -fno-PIE
endif
if ENABLE_BYPASS
GLIBC_CFLAGS += -DUKL_BP
else
GLIBC_CFLAGS += -UUKL_BP
endif

libc.a: stamp-glibc-configure
	rm -f $@
# XXX This fails in the iconv subdirectory, and there seems to be
# no way to disable that feature.  So instead just continue past
# the failure and hope if libc.a has not been created then the
# build will fail on the next line.
	-cd glibc-build && $(MAKE)
	cp glibc-build/libc.a $@

libpthread.a: stamp-glibc-configure libc.a
	rm -f $@
	cd glibc-build && $(MAKE) subdirs=nptl
	cp glibc-build/nptl/libpthread.a $@

libm.a: stamp-glibc-configure libpthread.a
	rm -f $@
	cd glibc-build && $(MAKE) subdirs=math
	cp glibc-build/math/libm.a $@

librt.a: stamp-glibc-configure libm.a
	rm -f $@
	cd glibc-build && $(MAKE) subdirs=rt
	cp glibc-build/rt/librt.a $@

libcrypt.a: stamp-glibc-configure libm.a
	rm -f $@
	cd glibc-build && $(MAKE) subdirs=crypt
	cp glibc-build/crypt/libcrypt.a $@

libcrypto.a libssl.a: 
	rm -f $@
	cd openssl-3.2.0 && ./config no-shared no-dso -mcmodel=kernel -mno-red-zone -static -fno-stack-protector -fno-PIC linux-x86_64 && $(MAKE)
	cp libcrypto.a libssl.a $(abs_top_builddir)

crt1.o crti.o crtn.o: librt.a
	rm -f $@
	cp glibc-build/csu/$(basename $@).o $@

stamp-glibc-configure: libcrypto.a libssl.a
	rm -f $@
	mkdir -p glibc-build
	cd glibc-build && \
	$(abs_top_srcdir)/glibc/configure \
	    CFLAGS="$(GLIBC_CFLAGS)" \
	    --prefix=$(abs_builddir) \
	    --enable-hacker-mode \
	    --enable-timezone-tools \
	    --disable-build-nscd \
	    --disable-default-pie \
	    --disable-pic \
	    --disable-pie \
	    --disable-nscd \
	    --disable-pt_chown \
	    --enable-static-nss \
	    --disable-shared \
	    --disable-tunables \
	    --disable-werror \
	    x86_64-ukl \
	    build_alias=x86_64-ukl \
	    host_alias=x86_64-ukl \
	    target_alias=x86_64-ukl
	touch $@

#----------------------------------------------------------------------
# Build Linux kernel linked with UKL.a

noinst_DATA += vmlinuz

vmlinuz: linux/arch/x86/boot/bzImage
	cp $< $@

linux/arch/x86/boot/bzImage: linux/.config UKL.a
	$(MAKE) -C $(srcdir)/linux olddefconfig O=$(abs_builddir)/linux
	$(MAKE) -C $(srcdir)/linux WERROR=0 O=$(abs_builddir)/linux

linux/.config: saveconfig
	rm -f $@
	cp $(srcdir)/saveconfig $@
	echo CONFIG_UNIKERNEL_LINUX=y >> $@
	echo CONFIG_UKL_TLS=y >> $@
	echo CONFIG_UKL_ARCHIVE_PATH=$(ARCHIVE) >> $@
	echo CONFIG_UKL_NAME=$(TRIGGER) >> $@
if ENABLE_SAME_STACK
	echo CONFIG_UKL_SAME_STACK=y >> $@
endif
if ENABLE_USE_RET
	echo CONFIG_UKL_USE_RET=y >> $@
endif
if ENABLE_USE_IST_PF
	echo CONFIG_UKL_USE_IST_PF=y >> $@
endif
if ENABLE_SHORTCUT
	echo CONFIG_UKL_DEEP_SHORTCUT=y >> $@
endif

#----------------------------------------------------------------------
# Compile the program.

UKL.a: $(PROGRAM)/UKL.a
	cp $< $@

$(PROGRAM)/UKL.a: $(CRT_OBJS) $(LIBC_OBJS)
	cd $(PROGRAM) && $(MAKE) UKL.a

#----------------------------------------------------------------------
# Per-program configuration and data files (if required).

$(PROGRAM)/data.tar.gz:
	rm -f $@
	-cd $(PROGRAM) && $(MAKE) data.tar.gz
	if ! test -f $@; then tar zcf $@ --no-recursion . ; fi

#----------------------------------------------------------------------
# Build the initramfs

noinst_DATA += ukl-initrd.cpio.xz

ukl-initrd.cpio.xz: $(PROGRAM)/data.tar.gz vmlinuz
	cp $(PROGRAM)/data.tar.gz initrd/
	$(MAKE) -C $(srcdir)/initrd $@
	cp initrd/$@ $@

#----------------------------------------------------------------------
# Boot the unikernel under qemu.

# Non-privileged:
boot: ukl-initrd.cpio.xz vmlinuz
	qemu-system-x86_64 \
	    -cpu host,-smap,-smep -accel kvm -m 8G \
	    -kernel vmlinuz \
	    -initrd ./ukl-initrd.cpio.xz \
	    -nodefaults -nographic -serial stdio \
	    -append "console=ttyS0 net.ifnames=0 biosdevname=0 nowatchdog clearcpuid=smap,smep mitigations=off mds=off ip=192.168.122.128:::255.255.255.0::eth0:none" \
	    -net user

# Privileged (probably required if the kernel is expecting incoming
# network connections).
boot-priv: ukl-initrd.cpio.xz vmlinuz
	sudo qemu-system-x86_64 \
	    -cpu host,-smap,-smep -accel kvm -m 4G \
	    -kernel vmlinuz \
	    -initrd ./ukl-initrd.cpio.xz \
	    -nodefaults -nographic -serial stdio \
	    -append "console=ttyS0 net.ifnames=0 biosdevname=0 nowatchdog clearcpuid=smap,smep mitigations=off mds=off ip=192.168.122.2::192.168.122.1:255.255.255.0:qemu-guest:eth0:none" \
	    -net nic -net bridge,br=virbr0

#----------------------------------------------------------------------
# Maintainers only!

# Check no files are missing from EXTRA_DIST rules, and that all
# generated files have been included in the tarball.  (Note you must
# have done 'make dist')
maintainer-check-extra-dist:
	@zcat $(PACKAGE_NAME)-$(VERSION).tar.gz | tar tf - | sort | \
	  sed 's,^$(PACKAGE_NAME)-$(VERSION)/,,' > tarfiles
	@git ls-files | \
	    sort > gitfiles
	@comm -13 tarfiles gitfiles > comm-out
	@echo Checking for differences between EXTRA_DIST and git ...
	@cat comm-out
	@[ ! -s comm-out ]
	@rm tarfiles gitfiles comm-out
	@echo PASS: EXTRA_DIST tests

# Commit everything in the current directory and set the commit
# message to the current version number.
maintainer-commit:
	git commit -a -m "Version $(VERSION)."

# Tag HEAD with the current version.
maintainer-tag:
	git tag -a v$(VERSION) -m "Version $(VERSION)." -f


# omarjarkas@omarjarkas-ThinkPad-P53:~/Desktop/SEED/seed_project/erun2/src/http$ ip route | grep default
# default via 10.35.12.1 dev enp0s31f6 proto dhcp metric 100 
# omarjarkas@omarjarkas-ThinkPad-P53:~/Desktop/SEED/seed_project/erun2/src/http$ cat /etc/resolv.conf
# # This is /run/systemd/resolve/stub-resolv.conf managed by man:systemd-resolved(8).
# # Do not edit.
# #
# # This file might be symlinked as /etc/resolv.conf. If you're looking at
# # /etc/resolv.conf and seeing this text, you have followed the symlink.
# #
# # This is a dynamic resolv.conf file for connecting local clients to the
# # internal DNS stub resolver of systemd-resolved. This file lists all
# # configured search domains.
# #
# # Run "resolvectl status" to see details about the uplink DNS servers
# # currently in use.
# #
# # Third party programs should typically not access this file directly, but only
# # through the symlink at /etc/resolv.conf. To manage man:resolv.conf(5) in a
# # different way, replace this symlink by a static file or a different symlink.
# #
# # See man:systemd-resolved.service(8) for details about the supported modes of
# # operation for /etc/resolv.conf.

# nameserver 127.0.0.53
# options edns0 trust-ad
# search eait.uq.edu.au
# omarjarkas@omarjarkas-ThinkPad-P53:~/Desktop/SEED/seed_project/erun2/src/http$ systemd-resolve --status
# systemd-resolve: command not found
# omarjarkas@omarjarkas-ThinkPad-P53:~/Desktop/SEED/seed_project/erun2/src/http$ ip addr show
# 1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
#     link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
#     inet 127.0.0.1/8 scope host lo
#        valid_lft forever preferred_lft forever
#     inet6 ::1/128 scope host 
#        valid_lft forever preferred_lft forever
# 2: enp0s31f6: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
#     link/ether 00:2b:67:35:28:4f brd ff:ff:ff:ff:ff:ff
#     inet 10.35.15.175/22 brd 10.35.15.255 scope global dynamic noprefixroute enp0s31f6
#        valid_lft 3490sec preferred_lft 3490sec
#     inet6 fe80::f485:5503:9d9b:5165/64 scope link noprefixroute 
#        valid_lft forever preferred_lft forever
# 3: virbr0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default qlen 1000
#     link/ether 52:54:00:27:66:b4 brd ff:ff:ff:ff:ff:ff
#     inet 192.168.122.1/24 brd 192.168.122.255 scope global virbr0
#        valid_lft forever preferred_lft forever
# 4: docker0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc noqueue state DOWN group default 
#     link/ether 02:42:56:62:6a:5b brd ff:ff:ff:ff:ff:ff
#     inet 172.17.0.1/16 brd 172.17.255.255 scope global docker0
#        valid_lft forever preferred_lft forever
# 32: tap0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel master virbr0 state UNKNOWN group default qlen 1000
#     link/ether fe:18:32:e5:ff:e7 brd ff:ff:ff:ff:ff:ff
#     inet6 fe80::fc18:32ff:fee5:ffe7/64 scope link 
#        valid_lft forever preferred_lft forever














# libz.a: stamp-glibc-configure
# 	rm -f $@
# 	cd zlib-1.3 && ./configure --static
# 	$(MAKE) CFLAGS="-mcmodel=kernel -fno-PIC -no-pie"
# 	cp libz.a $(abs_top_builddir)

# libdb.a: stamp-glibc-configure libz.a
# 	rm -f $@
# 	cd db-18.1.40/build_unix && ../dist/configure --enable-static --disable-shared
# 	$(MAKE)
# 	cp libdb.a $(abs_top_builddir)

# liblber.a: stamp-glibc-configure libdb.a
# 	rm -f $@
# 	cd openldap-2.5.16
# 	./configure --enable-static --disable-shared
# 	$(MAKE) depend
# 	$(MAKE) -C libraries/liblber
# 	cp libraries/liblber/.libs/liblber.a $(abs_top_builddir)

# libldap.a: liblber.a
# 	rm -f $@
# 	cd openldap-2.5.16
# 	$(MAKE) -C libraries/libldap
# 	cp libraries/libldap/.libs/libldap.a $(abs_top_builddir)

# libcrypto.a libssl.a: stamp-glibc-configure libldap.a
# 	rm -f $@
# 	cd openssl-3.2.0
# 	./config no-shared no-dso -fno-PIC
# 	$(MAKE) CFLAGS="-mcmodel=kernel -fno-PIC -no-pie"
# 	cp openssl-3.2.0/libcrypto.a openssl-3.2.0/libssl.a $(abs_top_builddir)

# libcurl.a: libssl.a libcrypto.a libldap.a liblber.a libdb.a libz.a
# 	rm -f $@
# 	cd curl-8.5.0
# 	./configure --disable-shared --enable-static --with-openssl
# 	$(MAKE) CFLAGS="-mcmodel=kernel -fno-PIC"
# 	cp lib/.libs/libcurl.a $(abs_top_builddir)


