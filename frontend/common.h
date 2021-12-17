#ifndef COMMON
#define COMMON

#define err(msg, ...)	{ \
	fprintf(stderr, msg, ##__VA_ARGS__); \
	exit(EXIT_FAILURE); \
}

uint16_t conv_from_hex(char *v)
{
	long long dec = 0;
	long long base = 1;

	for(int i = strlen(v); i >= 0; i--)
	{
		if(v[i] >= '0' && v[i] <= '9')
		{
			dec += (v[i]-48)*base;
			base *= 16;
		}
		else if(v[i] >= 'A' && v[i] <= 'F')
		{
			dec += (v[i]-55)*base;
			base*=16;
		}
		else if(v[i] >= 'a' && v[i] <= 'f')
		{
			dec += (v[i]-87)*base;
			base*=16;
		}
	}

	return dec;
}

#endif
