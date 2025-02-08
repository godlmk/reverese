// cbase.cpp: 定义应用程序的入口点。
//

#include <iostream>

using namespace std;
void bubbleSort(int arr[], int n)
{
	int i, j;
	for (i = 0; i < n - 1; i++)
	{
		for (j = 0; j < n - i - 1; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				int temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}
long long get() {
	long long ans = 0x0123456789abcdef;
	return ans;
}
void parameter() {
	char arr1[3] = { 0,1,2 };
	char arr2[4] = { 0,1,2,3 };
}

int main()
{
	//long long ll = get();
	parameter();
	return 0;
}
