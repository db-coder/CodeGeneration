int f()
{
	return 1;
}
int main()
{
    float a,b,c,d;
    a=(b+c)*(d+c);
    if (a==b)
    {
    	b=1;
    }
    else
    {
    	a=1;
    	f();
    }
    return a;
}