#pragma warning(disable:4996)
#include <stdio.h>

int ReadInt() {
	int ch = getchar();
	while ((ch > '9' || ch < '0') && ch != '-') {
		ch = getchar();
	}
	int ans = 0;
	int flag = 1;
	if (ch == '-') {
		flag = -1;
		ch = getchar();
	}
	while (ch >= '0' && ch <= '9') {
		ans = ans * 10 + ch - '0';
		ch = getchar();
	}
	return ans * flag;
}

int GetIntLength(int x) {
	int ans = 1;
	if (x < 0) {
		x = -x;
		ans = ans + 1;
	}
	if (x >= 10) {
		ans = ans + GetIntLength(x / 10);
	}
	return ans;
}

void PrintInt(int x) {
	if (x < 0) {
		putchar('-');
		x = -x;
	}
	if (x >= 10) {
		PrintInt(x / 10);
	}
	putchar(x % 10 + '0');
}

const int maxn = 25 + 10;
int n1, m1, a1[maxn][maxn];
int n2, m2, a2[maxn][maxn];
int n3, m3, a3[maxn][maxn];

int main() {
	//freopen("./input/task2.1.in", "r", stdin);
	n1 = ReadInt();
	m1 = ReadInt();
	//scanf("%d%d", &n1, &m1);

	int i = 1;
	while (i <= n1) {
		int j = 1;
		while (j <= m1) {
			a1[i][j] = ReadInt();
			j = j + 1;
			//scanf("%d", &a1[i][j]);
		}
		i = i + 1;
	}

	n2 = ReadInt();
	m2 = ReadInt();
	//scanf("%d%d", &n2, &m2);

	i = 1;
	while (i <= n2) {
		int j = 1;
		while (j <= m2) {
			a2[i][j] = ReadInt();
			j = j + 1;
			//scanf("%d", &a2[i][j]);
		}
		i = i + 1;
	}

	if (m1 != n2) {
		putchar('I');
		putchar('n');
		putchar('c');
		putchar('o');
		putchar('m');
		putchar('p');
		putchar('a');
		putchar('t');
		putchar('i');
		putchar('b');
		putchar('l');
		putchar('e');
		putchar(' ');
		putchar('D');
		putchar('i');
		putchar('m');
		putchar('e');
		putchar('n');
		putchar('s');
		putchar('i');
		putchar('o');
		putchar('n');
		putchar('s');
		putchar('\n');
		//printf("Incompatible Dimensions\n");
		return 0;
	}
	
	n3 = n1; m3 = m2;
	i = 1;
	while (i <= n3) {
		int j = 1;
		while (j <= m3) {
			int k = 1;
			while (k <= m1) {
				a3[i][j] += a1[i][k] * a2[k][j];
				k = k + 1;
			}
			j = j + 1;
		}
		i = i + 1;
	}

	i = 1;
	while (i <= n3) {
		int j = 1;
		while (j <= m3) {
			int len = GetIntLength(a3[i][j]);
			int k = 1;
			//printf("len = %d\n", len);
			while (k <= 10 - len) {
				putchar(' ');
				k = k + 1;
			}
			PrintInt(a3[i][j]);
			j = j + 1;
			//printf("%10d", a3[i][j]);
		}
		i = i + 1;
		putchar('\n');
	}
	
	return 0;
}