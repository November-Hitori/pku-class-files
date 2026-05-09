#include<bits/stdc++.h>
using namespace std;

using db=double;
using i64=long long;
using vi=vector<int>;
using pii=pair<int,int>;
using ull=unsigned long long;
using i128=__int128;

#define fi first
#define se second
#define gc getchar
#define pb emplace_back
#define mkp make_pair
#define push emplace
#define sz(a) (int)a.size()
#define all(p) p.begin(),p.end()
#define FOR(i,a,b) for(int i=a;i<=b;i++)
#define ROF(i,a,b) for(int i=a;i>=b;i--)
#define ppc(x) __builtin_popcount(x)
#define clz(x) __builtin_clz(x)

void chkmax(int &x,int y) {if(x<y) x=y;}
void chkmin(int &x,int y) {if(x>y) x=y;}
void chkmax(i64 &x,i64 y) {if(x<y) x=y;}
void chkmin(i64 &x,i64 y) {if(x>y) x=y;}
int read() { int x; cin>>x; return x; }

const int dx[8]={0,0,1,-1,1,-1,1,-1};
const int dy[8]={1,-1,0,0,1,-1,-1,1};
const int BLACK=1,WHITE=2,BLOCK=3,EMPTY=0,TMOVE=6,SIZE=8;
const db C=1.414,inf=1e9;

float k1[3] = {0.37, 0.25, 0.10};
float k2[3] = {0.14, 0.30, 0.80};
float k3[3] = {0.13, 0.20, 0.05};
float k4[3] = {0.13, 0.20, 0.05};
float k5[3] = {0.05, 0.05, 0.00};