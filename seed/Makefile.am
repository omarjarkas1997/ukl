
CLEANFILES = *~ src/*.o *.a src/undefined_sys_hack.c
SRCDIR = src
LIBDIR = $(SRCDIR)/liberun
YAJLDIR = $(SRCDIR)/yajl

clean-local:
	rm -f src/liberun/*.o

noinst_DATA = UKL.a

# Libraries built in the top build directory.
CRT_STARTS  = $(abs_top_builddir)/crt1.o $(abs_top_builddir)/crti.o \
	      $(abs_top_builddir)/crtbeginT.o
CRT_ENDS    = $(abs_top_builddir)/crtend.o $(abs_top_builddir)/crtn.o
C_LIB       = $(abs_top_builddir)/libc.a
PTHREAD_LIB = $(abs_top_builddir)/libpthread.a
RT_LIB      = $(abs_top_builddir)/librt.a
MATH_LIB    = $(abs_top_builddir)/libm.a
GCC_LIBS    = $(abs_top_builddir)/libgcc.a $(abs_top_builddir)/libgcc_eh.a

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/liberun

AM_CFLAGS   = -ggdb -mno-red-zone -mcmodel=kernel -static
if DEFAULT_PIE
AM_CFLAGS  += -fno-PIC -no-pie -fno-PIE
endif

data.tar.gz:
	touch data.tar.gz

UKL.a: src/erun.o src/undefined_sys_hack.o src/create.o \
	$(LIBDIR)/utils.o \
	$(LIBDIR)/runtime_spec_schema_config_schema.o \
	$(LIBDIR)/runtime_spec_schema_config_zos.o \
	$(LIBDIR)/error.o \
	$(LIBDIR)/chroot_realpath.o \
	$(LIBDIR)/runtime_spec_schema_defs_vm.o \
	$(LIBDIR)/runtime_spec_schema_defs_linux.o \
	$(LIBDIR)/runtime_spec_schema_config_linux.o \
	$(LIBDIR)/container.o \
	$(LIBDIR)/linux.o \
	$(LIBDIR)/terminal.o \
	$(LIBDIR)/cgroup.o \
	$(LIBDIR)/cgroup-cgroupfs.o \
	$(LIBDIR)/cgroup-resources.o \
	$(LIBDIR)/ebpf.o \
	$(LIBDIR)/cgroup-setup.o \
	$(LIBDIR)/cgroup-systemd.o \
	$(LIBDIR)/cgroup-utils.o \
	$(LIBDIR)/criu.o \
	$(LIBDIR)/intelrdt.o \
	$(LIBDIR)/io_priority.o \
	$(LIBDIR)/scheduler.o \
	$(LIBDIR)/status.o \
	$(LIBDIR)/mount_flags.o \
	$(LIBDIR)/cloned_binary.o \
	$(LIBDIR)/custom-handler.o \
	$(LIBDIR)/runtime_spec_schema_features_schema.o \
	$(LIBDIR)/runtime_spec_schema_config_solaris.o \
	$(LIBDIR)/runtime_spec_schema_config_windows.o \
	$(LIBDIR)/runtime_spec_schema_state_schema.o \
	$(LIBDIR)/json_common.o \
	$(LIBDIR)/read-file.o \
	$(LIBDIR)/runtime_spec_schema_defs_windows.o \
	$(LIBDIR)/runtime_spec_schema_config_vm.o \
	$(LIBDIR)/runtime_spec_schema_defs_zos.o \
	$(LIBDIR)/runtime_spec_schema_defs.o \
	$(LIBDIR)/runtime_spec_schema_features_linux.o \
	$(YAJLDIR)/yajl_alloc.o \
	$(YAJLDIR)/yajl_buf.o \
	$(YAJLDIR)/yajl.o \
	$(YAJLDIR)/yajl_gen.o \
	$(YAJLDIR)/yajl_lex.o \
	$(YAJLDIR)/yajl_parser.o \
	$(YAJLDIR)/yajl_tree.o \
	$(YAJLDIR)/yajl_version.o \
	$(YAJLDIR)/yajl_encode.o 
	rm -f $@ temp.o
	ld -r -o temp.o -allow-multiple-definition \
	    $(CRT_STARTS) \
	    src/erun.o \
	    --start-group \
	    --whole-archive \
	    $(MATH_LIB) $(RT_LIB) $(PTHREAD_LIB) $(C_LIB) \
	    --no-whole-archive \
	    $(GCC_LIBS) \
	    --end-group \
	    $(CRT_ENDS)
	ar cr UKL.a temp.o src/undefined_sys_hack.o src/create.o \
	$(LIBDIR)/utils.o \
	$(LIBDIR)/error.o \
	$(LIBDIR)/chroot_realpath.o \
	$(LIBDIR)/container.o \
	$(LIBDIR)/linux.o \
	$(LIBDIR)/terminal.o \
	$(LIBDIR)/cgroup.o \
	$(LIBDIR)/cgroup-cgroupfs.o \
	$(LIBDIR)/cgroup-resources.o \
	$(LIBDIR)/ebpf.o \
	$(LIBDIR)/cgroup-setup.o \
	$(LIBDIR)/cgroup-systemd.o \
	$(LIBDIR)/cgroup-utils.o \
	$(LIBDIR)/criu.o \
	$(LIBDIR)/intelrdt.o \
	$(LIBDIR)/io_priority.o \
	$(LIBDIR)/scheduler.o \
	$(LIBDIR)/status.o \
	$(LIBDIR)/mount_flags.o \
	$(LIBDIR)/cloned_binary.o \
	$(LIBDIR)/custom-handler.o \
	$(LIBDIR)/runtime_spec_schema_defs_vm.o \
	$(LIBDIR)/runtime_spec_schema_defs_linux.o \
	$(LIBDIR)/runtime_spec_schema_config_linux.o \
	$(LIBDIR)/runtime_spec_schema_config_schema.o \
	$(LIBDIR)/runtime_spec_schema_config_zos.o \
	$(LIBDIR)/runtime_spec_schema_features_schema.o \
	$(LIBDIR)/runtime_spec_schema_config_solaris.o \
	$(LIBDIR)/runtime_spec_schema_config_windows.o \
	$(LIBDIR)/runtime_spec_schema_state_schema.o \
	$(LIBDIR)/json_common.o \
	$(LIBDIR)/read-file.o \
	$(LIBDIR)/runtime_spec_schema_defs_windows.o \
	$(LIBDIR)/runtime_spec_schema_config_vm.o \
	$(LIBDIR)/runtime_spec_schema_defs_zos.o \
	$(LIBDIR)/runtime_spec_schema_defs.o \
	$(LIBDIR)/runtime_spec_schema_features_linux.o \
	$(YAJLDIR)/yajl_alloc.o \
	$(YAJLDIR)/yajl_buf.o \
	$(YAJLDIR)/yajl.o \
	$(YAJLDIR)/yajl_gen.o \
	$(YAJLDIR)/yajl_lex.o \
	$(YAJLDIR)/yajl_parser.o \
	$(YAJLDIR)/yajl_tree.o \
	$(YAJLDIR)/yajl_version.o \
	$(YAJLDIR)/yajl_encode.o 
	rm temp.o
	objcopy --prefix-symbols=ukl_ $@
	objcopy --redefine-syms=$(top_srcdir)/redef_sym_names $@


src/undefined_sys_hack.c: $(top_srcdir)/undefined_sys_hack.c
	cp $< $@



# automake doesn't add this rule unless we were to specify a C program
# to compile, which we don't want to do because of the very special
# linking requirements.
.c.o:
	$(CC) $(CFLAGS) $(AM_CFLAGS) -c $< -o $@
