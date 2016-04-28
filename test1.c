struct s
{
	float x;
	int y;	
};

int f()
{
	printf(12);
	return 1;
}
int main()
{
	struct s a,b;
	int x[10];
	b.x=1.7;
	b.y=2;
	a=b;
	x[1]=2;
	x[5]=7;
	f();
	printf("\n",x[1],"\n",x[5]);
	printf("\n",b.x,"\n",b.y);
	printf("\n",a.x,"\n",a.y);
    return 1;
}