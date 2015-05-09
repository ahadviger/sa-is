#include <cstdio>
#include <cstring>
#include <algorithm>
#include <stack>

using namespace std;

#define CHAR_NUMBER 100

typedef struct {
	int up, down, nextlindex;
} Child_Table;

void determine_char_type(int *S, int n, bool *char_type) {
	// 0 = L-type, 1 = S-type
	bool suffix_type = true;

	for(int i = n - 1; i >= 0; --i) {
		if(i == n - 1) {
			char_type[i] = true;
			continue;
		}
		if(S[i] == S[i+1]) {
			char_type[i] = suffix_type;
		} else {
			char_type[i] = S[i] < S[i+1];
			suffix_type = char_type[i];
		}
	}
}

void determine_LMS(bool *char_type, int n, int *LMS, int *n_LMS) {
	for(int i = 1; i < n; ++i) {
		if(char_type[i] && !char_type[i-1]) LMS[(*n_LMS)++] = i;
	}
}

bool is_LMS(bool *char_type, int i) {
	if(i == 0) return false;
	return(char_type[i] && !char_type[i-1]);
}

void initialize_buckets(int *S, int n, int *B, int *B_start) {
	int r = max(CHAR_NUMBER, n);
	for(int i = 0; i < r; ++i) {
		B[i] = 0;
	}
	for(int i = 0; i < n - 1; ++i) {
		++B[S[i]];
	}
	B_start[0] = 0;
	
	for(int i = 1; i < r; ++i) {
		B[i] += B[i-1];
		B_start[i] = B[i] != B[i-1] ? B[i-1] + 1 : 0;
	}
}

void induced_sort(int *SA, int *S, int *B, int *B_start, int n, int *LMS, int n_LMS, bool *char_type) {
	int B_end[100];
	for(int i = 0; i < n; ++i) {
		SA[i] = -1;
	}
	int r = max(CHAR_NUMBER, n);
	for(int i = 0; i < r; ++i) {
		B_end[i] = B[i];
	}
	
	for(int i = n - 1; i >= 0; --i) {
		if(is_LMS(char_type, i)) SA[B[S[i]]--] = i;
	}
		
	for(int i = 0; i < n; ++i) {
		if(SA[i] <= 0) continue;
		if(!char_type[SA[i] - 1]) {
			SA[B_start[S[SA[i] - 1]]++] = SA[i] - 1;
		}
	}
	
	for(int i = n - 1; i >= 0; --i) {
		if(SA[i] <= 0) continue;
		if(char_type[SA[i] - 1]) {
			SA[B_end[S[SA[i] - 1]]--] = SA[i] - 1;
		}
	}
}

void normalize_string(int *S, int n) {
	int min = S[0];
	for(int i = 1; i < n; ++i) {
		if(S[i] < min) min = S[i];
	}
	for(int i = 0; i < n; ++i) {
		S[i] = S[i] - min + 1;
	}
}

bool compare_LMS(int *S, bool *char_type, int n, int n1, int n2) {
	if(n1 == n - 1 || n2 == n - 1) {
		return n1 == n2;
	}
	int e1 = 0;
	int e2 = 0;
	for(e1 = n1 + 1; e1 < n; ++e1) {
		if(is_LMS(char_type, e1)) break;
	}
	for(e2 = n2 + 1; e2 < n; ++e2) {
		if(is_LMS(char_type, e2)) break;
	}
	if(e1 - n1 != e2 - n2) return false;
	for(int i = 0; i < e1 - n1 + 1; ++i) {
		if(S[n1+i] != S[n2+i] || char_type[n1+i] != char_type[n2+i]) {
			return false;
		}
	}
	return true;
}

void name_substrings(int *S1, int *SA, int *S, int *LMS, int n, int n_LMS, bool *char_type, bool *unique) {
	int name = 0;
	int *named = (int*) malloc(n * sizeof(int));
	int last = -1;
	*unique = true;
	
	for(int i = 0; i < n; ++i) {
		if(!is_LMS(char_type, SA[i])) continue;
		if(i == 0) {
			named[SA[i]] = name;
			last = SA[i];
			continue;
		}
		if(!compare_LMS(S, char_type, n, SA[i], last)) {
			++name;
		} else {
			*unique = false;
		}
		named[SA[i]] = name;
		last = SA[i];
	}
	
	for(int i = 0; i < n_LMS; ++i) {
		S1[i] = named[LMS[i]];
	}
	
	free(named);
}

bool same_buckets(int a, int b, int *B, int n) {
	int r = max(CHAR_NUMBER, n);
	for(int i = 0; i < r; ++i) {
		if(a < B[i] && b >= B[i] || b < B[i] && a >= B[i]) return false;
	}
	return true;
}

void determine_SA(char *SS, int *SA, int *S, int *SA1, int *LMS, int *B, int *B_start, bool *char_type, int n, int n_LMS) {
	int B_end[100];
	for(int i = 0; i < n; ++i) {
		SA[i] = -1;
	}
	int r = max(CHAR_NUMBER, n);
	for(int i = 0; i < r; ++i) {
		B_end[i] = B[i];
	}

	for(int i = n_LMS - 1; i >= 0; --i) {
		SA[B[S[LMS[SA1[i]]]]--] = LMS[SA1[i]];
	}
	
	for(int i = 0; i < n; ++i) {
		if(SA[i] <= 0) continue;
		if(!char_type[SA[i] - 1]) {
			SA[B_start[S[SA[i] - 1]]++] = SA[i] - 1;
		}
	}

	for(int i = n - 1; i >= 0; --i) {
		if(SA[i] <= 0) continue;
		if(char_type[SA[i] - 1]) {
			SA[B_end[S[SA[i] - 1]]--] = SA[i] - 1;
		}
	}
}

// ovo ne radi
void determine_SA_LCP(int *SA, int *LCP, int *S, int *SA1, int *LMS, int *B, int *B_start, bool *char_type, int n, int n_LMS) {
	int B_end[100];
	int SA_i[100];
	int B_init[100];
	for(int i = 0; i < n; ++i) {
		SA[i] = -1;
		B_end[i] = B[i];
		SA_i[i] = -1;
		B_init[i] = B_start[i];
	}

	for(int i = n_LMS - 1; i >= 0; --i) {
		SA[B[S[LMS[SA1[i]]]]--] = LMS[SA1[i]];
	//	LCP[B[S[LMS[SA1[i]]]] + 1] = LCP_LMS[SA1[i]];
		LCP[B[S[LMS[SA1[i]]]] + 1] = 0;
	}
		
	for(int i = 0; i < n; ++i) {
		if(SA[i] <= 0) continue;
		if(!char_type[SA[i] - 1]) {
			SA[B_start[S[SA[i] - 1]]] = SA[i] - 1;
			if(SA_i[S[SA[i] - 1]] == -1) {
				LCP[B_start[S[SA[i] - 1]]] = 0;
			} else {
				int ii = SA_i[S[SA[i] - 1]];
				if(same_buckets(i, ii, B_init, n)) {
					int min = LCP[ii + 1];
					for(int j = ii + 1; j <= i; ++j) {
						if(LCP[j] < min) min = LCP[j];
					}
					LCP[B_start[S[SA[i] - 1]]] = min + 1;
				} else {
					LCP[B_start[S[SA[i] - 1]]] = 1;
				}
			}
			SA_i[S[SA[i] - 1]] = i;
			B_start[S[SA[i] - 1]]++;
		}
	}
	
	for(int i = 0; i < n; ++i) {
		SA_i[i] = -1;
	}

	for(int i = n - 1; i >= 0; --i) {
		if(SA[i] <= 0) continue;
		if(char_type[SA[i] - 1]) {
			SA[B_end[S[SA[i] - 1]]] = SA[i] - 1;
			if(SA_i[S[SA[i] - 1]] == -1) {
				LCP[B_end[S[SA[i] - 1]]] = 0;
			} else {
				int ii = SA_i[S[SA[i] - 1]];
				if(same_buckets(i, ii, B_init, n)) {
					int min = LCP[i + 1];
					for(int j = i + 1; j <= ii; ++j) {
						if(LCP[j] < min) min = LCP[j];
					}
					LCP[B_end[S[SA[i] - 1]]] = min + 1;
				} else {
					LCP[B_end[S[SA[i] - 1]]] = 1;
				}
			}
			SA_i[S[SA[i] - 1]] = i;
			B_end[S[SA[i] - 1]]--;
		}
	}
}

void print_sorted(char *S, int *SA, int n) {
	for(int i = 1; i < n + 1; ++i) {
		 printf("%s\n", S + SA[i]);
	}
}

void solve(char *SS, int *S, int *SA, int n) {
	// printf("ulaz u solve\n");
	bool s[10000];
	int LMS[100];
	int B[100];
	int B_start[100];
	int S1[100];
	int SA1[100];
	int n_LMS = 0;
	bool unique;
	
	normalize_string(S, n);
	S[n++] = 0;
	determine_char_type(S, n, s);
	determine_LMS(s, n, LMS, &n_LMS);
	initialize_buckets(S, n, B, B_start);
	induced_sort(SA, S, B, B_start, n, LMS, n_LMS, s);
	name_substrings(S1, SA, S, LMS, n, n_LMS, s, &unique);
	
	if(!unique) {
		solve(SS, S1, SA1, n_LMS);
		for(int i = 1; i < n_LMS + 1; ++i) {
			SA1[i-1] = SA1[i];
		}
	} else {
		for(int i = 0; i < n_LMS; ++i) {
			SA1[S1[i]] = i;
		}
	}
	initialize_buckets(S, n, B, B_start);
	determine_SA(SS, SA, S, SA1, LMS, B, B_start, s, n, n_LMS);
}

void convert_to_int(char *SS, int *S, int n) {
	for(int i = 0; i < n; ++i) {
		S[i] = SS[i];
	}
}

void determine_ISA(int *ISA, int *SA, int n) {
	for(int i = 0; i < n; ++i) {
		ISA[SA[i]] = i;
	}
}

void determine_LCP(int *LCP, int *S, int *SA, int *ISA, int n) {
	if(ISA == NULL) {
		determine_ISA(ISA, SA, n);
	}
	
	int l = 0;
	for(int i = 0; i < n; ++i) {
		int k = ISA[i];
		int j = SA[k-1];
		while(i + l < n && j + l < n && S[i+l] == S[j+l]) {
			++l;
		}
		LCP[k] = l;
		if(l > 0) --l;
	}
}

void construct_child_table(Child_Table *CT, int *LCP, int n) {
	int last_index = -1;
	stack<int> stack;
	stack.push(0);
	for(int i = 1; i < n; ++i) {
		while(LCP[i] < LCP[stack.top()]) {
			last_index = stack.top();
			stack.pop();
			if(LCP[i] <= LCP[stack.top()] && LCP[stack.top()] != LCP[last_index]) {
				CT[stack.top()].down = last_index;
			}
		}
		if(LCP[i] >= LCP[stack.top()]) {
			if(last_index != -1) {
				CT[i].up = last_index;
				last_index = -1;
			}
			stack.push(i);
		}
	}
	
	while(!stack.empty()) stack.pop();
	
	stack.push(0);
	for(int i = 1; i < n; ++i) {
		while(LCP[i] < LCP[stack.top()]) {
			stack.pop();
		}
		if(LCP[i] == LCP[stack.top()]) {
			last_index = stack.top();
			stack.pop();
			CT[last_index].nextlindex = i;
		}
		stack.push(i);
	}
}

int search(char *S, char *pattern, int *SA, int n) {
	int m = strlen(pattern);
	int l = 0, r = n - 1;
	while (l <= r) {
		int mid = l + (r - l)/2;
		int res = strncmp(pattern, S + SA[mid], m);
        if (res == 0) {
			return SA[mid];
		}
		if (res < 0) {
			r = mid - 1;
		} else {
			l = mid + 1;
		}
	}
	return -1;
}

int main(void) {

	char SS[10000] = {1, 3, 1, 2, 0};
	char p[1000];
	int n;
	scanf("%s", SS);
	n = strlen(SS);
//	scanf("%s", p);
	int S[10000];
	convert_to_int(SS, S, n);
	normalize_string(S, n);
	int SA[100];
	int ISA[100];
	int LCP[100];
	solve(SS, S, SA, n);
//	determine_ISA(ISA, SA, n + 1);
//	determine_LCP(LCP, S, SA, ISA, n + 1);
//	for(int i = 1; i < n + 1; ++i)  printf("%d\n", SA[i]);
/*	// printf("\n");
	for(int i = 0; i < n + 1; ++i)  printf("%d ", LCP[i]);
	// printf("\n");*/
	print_sorted(SS, SA, n);
	/*

	Child_Table CT[100];
	construct_child_table(CT, LCP, n + 1);
	for(int i = 0; i < n; ++i) {
		// printf("%d %d %d\n", CT[i].up, CT[i].down, CT[i].nextlindex);
	}*/
	
	return 0;
}