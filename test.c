int f()
{
	return 1;
}
int main()
{
    int a,b,c,d;
    a=(b+c)*(d+c);
    if (a==b)
    {
    	b=1;
    }
    else
    {
    	a=1;
    }
    return a;
}