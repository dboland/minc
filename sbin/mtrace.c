#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/mman.h>

#include "win/windows.h"
#include "win/winternl.h"
#include "win_posix.h"
#include "vfs_posix.h"
#include "minc_posix.h"

struct _NT_TIB *NtCurrentTeb(void);

extern unsigned char _bss_end__;
extern unsigned char _data_end__;
extern unsigned char _data_start__;
extern unsigned char __tls_end__;
extern unsigned char __tls_start__;

extern unsigned char _end__;
extern unsigned char _bss_start__;
extern unsigned char etext;
extern unsigned char _start;
extern unsigned char _image_base__;

extern unsigned char _size_of_stack_reserve__;
extern unsigned char _size_of_heap_reserve__;

extern __import void *__Session;
extern __import WIN_TASK *__Tasks;

int bx_get(void);
int cx_get(void);
int dx_get(void);

int cs_get(void);
int ds_get(void);
int es_get(void);
int fs_get(void);
int gs_get(void);
int ss_get(void);

int esp_get(void);

void gs_set(int val);
void ds_set(int val);

int ds_read(void *offset);
int cs_read(void *offset);
int fs_read(void *offset);
int gs_read(void *offset);

__thread int global_var[4];

void 
print_system(PNT_TIB Info)
{
	printf("------system\n");
	printf("%08x: _tib_start\n", Info->Self);
}
void 
print_stack(PNT_TIB Info)
{
	int local_var = -5;

	printf("------stack\n");
	printf("%08x: _stack_base\n", Info->StackBase);
	printf("%08x: _local_var(%d)\n", &local_var, local_var);
	printf("%08x: _program_counter\n", esp_get());
	printf("%08x: _stack_limit\n", Info->StackLimit);
}
void 
print_data(PNT_TIB Info)
{
	printf("------data\n");
//	printf("%08x: _global_var\n", global_var);
	printf("%08x: _global_var(%d)\n", global_var, global_var[2]);
}
void 
print_session(PNT_TIB Info)
{
	printf("------session\n");
	printf("%08x: _Session_end\n", __Session + sizeof(WIN_SESSION));
	printf("%08x: _Session_start\n", __Session);

}
void 
print_context(const char *label)
{
	printf("------context\n");
	// this clears %gs!
//	win_ldt_read((HANDLE)-2, 0x28, label);	/* 5: Task State Segment (TSS) */
	win_ldt_read((HANDLE)-2, cs_get(), label);
	win_ldt_read((HANDLE)-2, ds_get(), label);
	win_ldt_read((HANDLE)-2, fs_get(), label);

	printf("%08x: _cs_get__\n", cs_get());
	printf("%08x: _ds_get__\n", ds_get());
	printf("%08x: _es_get__\n", es_get());
	printf("%08x: _fs_get__\n", fs_get());
	printf("%08x: _gs_get__\n", gs_get());
	printf("%08x: _ss_get__\n", ss_get());

	printf("%08x: _bx_get__\n", bx_get());
	printf("%08x: _cx_get__\n", cx_get());
	printf("%08x: _dx_get__\n", dx_get());

}
void 
print_proc(PNT_TIB Info)
{
	int offset = 0x00102000;

	printf("------proc\n");

	printf("%08x: _end__\n", &_end__);

	printf("%08x: _tls_end__\n", &__tls_end__);
	printf("%08x: _tls_start__\n", &__tls_start__);

	printf("%08x: _bss_end__\n", &_bss_end__);
	printf("%08x: _bss_start__\n", &_bss_start__);

	printf("%08x: _data_end__\n", &_data_end__);
	printf("%08x: _data_start__\n", &_data_start__);

	printf("%08x: etext\n", &etext);
	printf("%08x: _start\n", &_start);

	printf("%08x: _image_base__\n", &_image_base__);

	printf("%08x: _size_of_stack_reserve__\n", &_size_of_stack_reserve__);
	printf("%08x: _size_of_heap_reserve__\n", &_size_of_heap_reserve__);

}
int 
main(int argc, char *argv, char *env)
{
//	setlocale(LC_NUMERIC, "");
	PNT_TIB Info;
	pid_t pid;
	int status;
//	char *ptr = (char *)0x201000;
	char *ptr = (char *)0x1000;

	Info = NtCurrentTeb();
	print_system(Info);

//	gs_set(ds_get());

//	if (mmap(ptr, 0x1000, PROT_READ | PROT_WRITE, MAP_ANON, 0, 0) < 0){
//		printf("mmap(0x%x: %s\n", ptr, strerror(errno));
//	}

//	win_ldt_write(0x27, 0x200000, 0x1000);

	switch (pid = fork()){		/* fork() closes pipes too soon? */
		case -1:
			printf("fork(): %s\n", strerror(errno));
		case 0:
//			win_ldt_write(0x27, 0x201000, 0x1000);	// clears %gs
			Info = NtCurrentTeb();
			global_var[2] = -1;
			print_system(Info);
			print_stack(Info);
			print_data(Info);
			print_context("child");
			break;
		default:
			waitpid(pid, &status, 0);
			print_stack(Info);
			print_session(Info);
			print_data(Info);
			print_context("parent");
			print_proc(Info);
	}

}

__asm(".text\n"			\
"_NtCurrentTeb:\n"		\
	"mov %fs:0x18,%eax\n"	\
	"ret\n"			\
"_esp_get:\n"			\
	"mov %esp,%eax\n"	\
	"ret\n"			\
"_cs_get:\n"			\
	"mov %cs,%eax\n"	\
	"ret\n"			\
"_ds_get:\n"			\
	"mov %ds,%eax\n"	\
	"ret\n"			\
"_es_get:\n"			\
	"mov %es,%eax\n"	\
	"ret\n"			\
"_fs_get:\n"			\
	"mov %fs,%eax\n"	\
	"ret\n"			\
"_gs_get:\n"			\
	"mov %gs,%eax\n"	\
	"ret\n"			\
"_ss_get:\n"			\
	"mov %ss,%eax\n"	\
	"ret\n"			\
"_bx_get:\n"			\
	"mov %ebx,%eax\n"	\
	"ret\n"			\
"_cx_get:\n"			\
	"mov %ecx,%eax\n"	\
	"ret\n"			\
"_dx_get:\n"			\
	"mov %edx,%eax\n"	\
	"ret\n"			\
"_cs_read:\n"			\
	"mov 4(%esp),%ecx\n"	\
	"mov %cs:(%ecx),%eax\n"	\
	"ret\n"			\
"_ds_read:\n"			\
	"mov 4(%esp),%ecx\n"	\
	"mov %ds:(%ecx),%eax\n"	\
	"ret\n"			\
"_fs_read:\n"			\
	"mov 4(%esp),%ecx\n"	\
	"mov %fs:(%ecx),%eax\n"	\
	"ret\n"			\
"_gs_read:\n"			\
	"mov 4(%esp),%ecx\n"	\
	"mov %gs:(%ecx),%eax\n"	\
	"ret\n"			\
"_stack_end:\n"			\
	"mov %fs:0x04,%eax\n"	\
	"ret\n"			\
"_stack_start:\n"		\
	"mov %fs:0x08,%eax\n"	\
	"ret\n"			\
"_gs_set:\n"			\
	"mov 4(%esp),%gs\n"	\
	"ret\n"			\
"_ds_set:\n"			\
	"mov 4(%esp),%ds\n"	\
	"ret\n"			\
);
