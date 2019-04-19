#include <cstdio>
#include "LinkListSimple.hpp"

using namespace std;
int main()
{
	LinkList<char> l = LinkList<char>();
	
	char buf = 'a';
	while((buf = getchar())!='\n')
	{
		l.insertAsFirst(buf);
		printf("#");
	}
	printf("hhh");
	while(l.size() != 0)
	{
		putchar(l.remove(l.first()));
	}
	return 0;
}

