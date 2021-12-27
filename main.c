#include <stdio.h>
#include "backend/read_file.h"
#include "backend/mini_processor.h"
#include "frontend/compiler.h"

int main(int args, char *argv[])
{

	char *file_info = read_file(argv[1]);
	Proc_t *proc = init_proc(file_info);

	get_next(proc);
	compile(blocks, ind);
}
