#ifndef MINI_P
#define MINI_P
#include <string.h>
#include "pre_parser.h"

/*
 * Proc - Struct the contains all information about the current status in lexing.
 * */
typedef struct Proc
{
	char		*data;
	char		curr;
	int		index;
	int		line;
	struct Proc	*last;
} Proc_t;

/*
 * TBlock - Stores information about the token.
 * */
typedef struct TBlock
{
	enum {
		mov,
		r_ax,
		r_ah,
		r_al,
		user_defined,
		Tdb,
		Tdw,
		Tdd,
		hex,
		dec,
		quote,
		eof
	} token_id;

	char *token_value;
	enum {
		r8,
		r16,
		r32,
		UD,
		none
	} op_i;

	enum {
		imm8,
		imm16,
		imm32,
		_UD,
		_none
	} op_s;
} TBlock_t;

/*
 * PBlock - Stores the overall information the lexer picked up.
 * */
typedef struct PBlock
{
	Proc_t		*curr_proc;
	TBlock_t	*curr_tblock;
	BinaryInfo_t	*binary_info;
} PBlock_t;

static size_t		ind = 0;
static PBlock_t		**blocks;

/*	Functions For The "backend" of the language	*/
Proc_t *init_proc(char *data)
{
	Proc_t *proc = calloc(1, sizeof(*proc));

	blocks = calloc(1, sizeof(*blocks));
	blocks[ind] = calloc(1, sizeof(*blocks[ind]));

	init_regs();

	proc->data = data;
	proc->index = 0;
	proc->curr = proc->data[proc->index];
	proc->last = (void*)0;
	proc->line = 1;

	return proc;
}

void advance(Proc_t *proc)
{
	proc->index++;
	proc->curr = proc->data[proc->index];
}

char *get_word(Proc_t *proc)
{
	char *word = calloc(1, sizeof(*word));
	int index = 0;

	while(
	     (proc->curr != ' ' && proc->curr != '\0') && 
	     (proc->curr >= 'a' && proc->curr <= 'z'))
	{
		word[index] = proc->curr;
		index++;
		word = realloc(word, (index+1)*sizeof(*word));
		advance(proc);
	}

	return word;
}

void init_token(int token_id, char *value, Proc_t *proc)
{
	TBlock_t *tblock = calloc(1, sizeof(*tblock));

	tblock->token_id = token_id;
	tblock->token_value = value;
	
	switch(tblock->token_id)
	{
		case mov:
		{
			proc->last = proc;
			tblock->op_i = none;
			tblock->op_s = _none;

			blocks[ind]->curr_proc = proc->last;
			blocks[ind]->curr_tblock = tblock;
			blocks[ind]->binary_info = (void *)0;
			ind++;

			blocks = realloc(blocks, (ind+1) * sizeof(*blocks));
			blocks[ind] = calloc(1, sizeof(*blocks[ind]));
			break;
		}
		case r_ax:
		{
			if(proc->last)
			{
				if(blocks[ind-1]->curr_tblock->token_id == mov)
				{
					tblock->op_i = r16;
					tblock->op_s = imm16;

					blocks[ind]->curr_proc = proc;
					blocks[ind]->curr_tblock = tblock;
					blocks[ind]->binary_info = calloc(1, sizeof(blocks[ind]->binary_info));
					ind++;

					blocks = realloc(blocks, (ind+1) * sizeof(*blocks));
					blocks[ind] = calloc(1, sizeof(*blocks[ind]));
				}
				
				proc->last = (void *)0;
				break;
			}
			
			fprintf(stderr, "Error, Line %d:\n\tUnknown register, %s", proc->line, value);
			exit(EXIT_FAILURE);

			break;

		}
		case r_ah:
		{
			if(proc->last)
			{
				if(blocks[ind-1]->curr_tblock->token_id == mov)
				{
					tblock->op_i = r8;
					tblock->op_s = imm8;

					blocks[ind]->curr_proc = proc;
					blocks[ind]->curr_tblock = tblock;
					blocks[ind]->binary_info = calloc(1, sizeof(blocks[ind]->binary_info));
					ind++;

					blocks = realloc(blocks, (ind+1) * sizeof(*blocks));
					blocks[ind] = calloc(1, sizeof(*blocks[ind]));
				}
				
				proc->last = (void *)0;
				break;
			}
			
			fprintf(stderr, "Error, Line %d:\n\tUnknown register, %s", proc->line, value);
			exit(EXIT_FAILURE);

			break;
		}

		case r_al:
		{
			if(proc->last)
			{
				if(blocks[ind-1]->curr_tblock->token_id == mov)
				{
					tblock->op_i = r8;
					tblock->op_s = imm8;

					blocks[ind]->curr_proc = proc;
					blocks[ind]->curr_tblock = tblock;
					blocks[ind]->binary_info = calloc(1, sizeof(blocks[ind]->binary_info));
					ind++;

					blocks = realloc(blocks, (ind+1) * sizeof(*blocks));
					blocks[ind] = calloc(1, sizeof(*blocks[ind]));
				}
				
				proc->last = (void *)0;
				break;
			}
			
			fprintf(stderr, "Error, Line %d:\n\tUnknown register, %s", proc->line, value);
			exit(EXIT_FAILURE);

			break;
		}

		case hex:
		{
			tblock->op_i = none;
			tblock->op_s = _none;

			blocks[ind]->curr_proc = proc;
			blocks[ind]->curr_tblock = tblock;
			blocks[ind]->binary_info = (void*)0;
			ind++;

			blocks = realloc(blocks, (ind+1) * sizeof(*blocks));
			blocks[ind] = calloc(1, sizeof(*blocks[ind]));
			break;
		}
		case user_defined:
		{
			tblock->op_i = UD;
			tblock->op_s = _UD;

			blocks[ind]->curr_proc = proc;
			blocks[ind]->curr_tblock = tblock;
			blocks[ind]->binary_info = calloc(1, sizeof(*blocks[ind]->binary_info));
			blocks[ind]->binary_info->opcode = user_def;

			_user_defined[udind]->name = tblock->token_value;
			ind++;
			blocks = realloc(blocks, (ind+1) * sizeof(*blocks));
			blocks[ind] = calloc(1, sizeof(*blocks[ind]));
			break;
		}
		case Tdb:
		{
			tblock->op_i = none;
			tblock->op_s = _none;

			blocks[ind]->curr_proc = proc;
			blocks[ind]->curr_tblock = tblock;
			blocks[ind]->binary_info = (void*)0;

			ind++;
			blocks = realloc(blocks, (ind+1) * sizeof(*blocks));
			blocks[ind] = calloc(1, sizeof(*blocks[ind]));
			break;
		}
		case quote:
		{
			tblock->op_i = none;
			tblock->op_s = _none;

			blocks[ind]->curr_proc = proc;
			blocks[ind]->curr_tblock = tblock;
			blocks[ind]->binary_info = (void*)0;

			ind++;
			blocks = realloc(blocks, (ind+1) * sizeof(*blocks));
			blocks[ind] = calloc(1, sizeof(*blocks[ind]));
			break;
		}
		default: break;
	}
	advance(proc);
}

char *get_value(Proc_t *proc)
{
	char *val = calloc(1, sizeof(*val));
	int index = 0;

	while(proc->curr != ' ' && proc->curr != '\0' && proc->curr != '\n')
	{
		val[index] = proc->curr;
		index++;
		val = realloc(
			val,
			(index+1)*sizeof(*val)
		);
		advance(proc);
	}

	return val;

}

char *get_quote(Proc_t *proc)
{
	char *val = calloc(1, sizeof(*val));
	int index = 0;

	while(proc->curr != '\'')
	{
		val[index] = proc->curr;
		index++;
		val = realloc(
			val,
			(index+1)*sizeof(*val)
		);
		advance(proc);
		if(proc->curr == '\'')
			break;
		if(proc->curr == '\n' || proc->curr == '\0')
		{
			fprintf(stderr, "Invalid syntax for user-defined assignment");
			exit(EXIT_FAILURE);
		}
	}
	//val[index++] = '\0';
	advance(proc);
	//printf("%ld", strlen(val));

	return val;
}

void get_next(Proc_t *proc)
{
	while(proc->index <= strlen(proc->data) && !(proc->curr == '\0'))
	{
redo:
		if(proc->curr == '\n')
		{
			while(proc->curr == '\n')
			{
				proc->line++;
				advance(proc);
			}
		}

		if(proc->curr == ' ')
		{
			while(proc->curr == ' ')
				advance(proc);
		}

		if(proc->curr >= 'a' && proc->curr <= 'z')
		{
			char *word = get_word(proc);
	
			if(strcmp(word, "mov")==0)
			{
				init_token(mov, word, proc);
				goto redo;
			}
			if(strcmp(word, "ax")==0)
			{
				init_token(r_ax, word, proc);
				goto redo;
			}
			if(strcmp(word, "ah")==0)
			{
				init_token(r_ah, word, proc);
				goto redo;
			}
			if(strcmp(word, "al")==0)
			{
				init_token(r_al, word, proc);
				goto redo;
			}
			if(strcmp(word, "db")==0)
			{
				init_token(Tdb, word, proc);
				goto redo;
			}
			
			init_token(user_defined, word, proc);
			goto redo;
		}
		if(proc->curr == '\'')
		{
			advance(proc);

			char *q = get_quote(proc);

			init_token(quote, q, proc);

			goto redo;
		}
		if(proc->curr >= '0' && proc->curr <= '9')
		{
			char *val = get_value(proc);

			for(size_t i = 0; i < strlen(val); i++)
			{
				if(val[i] == 'x' || val[i] == 'h')
				{
					init_token(hex, val, proc);
					goto redo;
				}
			}
			init_token(dec, val, proc);
			goto redo;
		}

		advance(proc);
	}

	if(blocks[ind])
	{
		TBlock_t	*tblock = calloc(1, sizeof(*tblock));
		tblock->token_id	= eof;
		tblock->token_value	= "\0";
		blocks[ind]->curr_tblock = tblock;
		blocks[ind]->curr_proc = proc;

		blocks[ind+1] = (void*)0;
		ind++;
	}
}

#endif
