static int PrintArr(int a[100], int n)
{
	int i = 0;
	while (i < n)
	{
		putch(a[i]);
		i = i + 1;
	}
	return 0;
}

static int main()
{
	int n[15] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!', 10};
	PrintArr(n, 14);
	return 0;
}