int sum(int b[])
{
    return b[0]+1;
}

int main()
{

    int a[1];
    a[0] = 1;
    sum(a);
	return a[0];
}