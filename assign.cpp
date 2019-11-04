//References
//https://timmurphy.org/2014/04/26/using-fork-in-cc-a-minimum-working-example/
//https://stackoverflow.com/questions/19675134/how-to-spawn-multiple-threads-that-call-same-function-using-stdthread-c
#include<iostream>
#include<thread>
#include<sstream>
#include<fstream>
#include<cstdlib>
#include<assert.h>
#include<sys/wait.h>
#include<unistd.h>
#include<cstring>
using namespace std;

int upperLimit=123;

void entireFiles(int threadId, int threadSum[], int start, int end, int type, int count){
	int sum=0;
	string filename;
	string prefix;
	if(type==1){
		prefix="a_";
	}
	else{
		prefix="b_";
	}
	ifstream f1;
	int val;
	for(int i=start;i<end;i++){
		stringstream s("");
		s<<prefix<<i<<".txt";
		filename=s.str();
		f1.open(filename.c_str());
		for(int j=0;j<count;j++){
			f1>>val;
			sum+=val;
		}
		f1.close();
	}
	threadSum[threadId] = sum;
}


void partialFiles(int threadId, int threadSum[], int prevend, int prevFileNumber, int end, int fileNumber, int count, int type )
{
	int sum=0;
	string filename;
	string prefix;
	if(type==1){
		prefix="a_";
	}
	else{
		prefix="b_";
	}
	ifstream f1;
	stringstream s("");
	s<<prefix<<prevFileNumber<<".txt";
	filename=s.str();
	int val;
	f1.open(filename.c_str());
	for(int i=0;i<prevend;i++){
		f1>>val;
	}

	
	bool sameFile=true;
	if(prevFileNumber==fileNumber){
		sameFile=true;

	}
	else{
		sameFile=false;
	}
	if(sameFile){

		for(int i=prevend;i<=end;i++){
			f1>>val;
			sum+=val;

		}
	}
	else{
		for(int i=prevend; i<count; i++){
			f1>>val;
			sum+=val;
		}
		f1.close();
		ifstream f2;
		stringstream s("");
		s<<prefix<<fileNumber<<".txt";
		filename=s.str();
		f2.open(filename.c_str());
		for(int i=0;i<=end;i++){
			f2>>val;
			sum+=val;
		}
		f2.close();
	}
	threadSum[threadId]=sum;

}



void spawn(int type, int fileCount, int threadCount, int threadSum[], int count){
	thread temp[threadCount];
	if(fileCount<threadCount){
		int total=count*fileCount;
		int perThread=total/threadCount;
		int extras=total%threadCount;
		int end, prevend;
		prevend=0;
		int fileNumber;
		int prevFileNumber=0;
		for(int i=0;i<threadCount;i++){
			end=prevend+perThread;
			if(i>=extras){
				end-=1;
			}
			fileNumber=prevFileNumber+(end/count);
			end=end%count;

			temp[i]=thread(partialFiles, i, threadSum, prevend, prevFileNumber, end,fileNumber, count, type);
			prevend=end+1;
			
			prevFileNumber=fileNumber+prevend/count;
			prevend=prevend % count;

		}

	}
	else{

		int start=0,end;
		int perThread=fileCount/threadCount;
		int extras=fileCount%threadCount;
		for(int i=0;i<threadCount;i++){
			end=start+perThread;
			if(i<extras){
				end+=1;
			}
			temp[i]=thread(entireFiles, i, threadSum, start, end, type, count);
			start=end;
		}

	}
	for(int i=0;i<threadCount;i++){
		temp[i].join();
	}
	return;



}


int createFiles(int nfilea, int nfileb, int count, int actual_sum){
	string filename;
	ofstream f1;
	ofstream f2;
	//Generating files that starts with a_
	for(int i=0;i<nfilea;i++){
		stringstream s("");
		s<<"a_"<<i<<".txt";
		filename=s.str();
		f1.open(filename.c_str());
		for(int j=0;j<count;j++){
			int x=rand()%upperLimit;
			f1<<x<<" ";
			actual_sum+=x;
		}
		f1.close();
		
	}
	cout<<nfilea<<" a_x files created succesfully"<<endl;
	//generating files that starts with b_
	for(int i=0;i<nfileb;i++){
		stringstream s("");
		s<<"b_"<<i<<".txt";
		filename=s.str();
		f2.open(filename.c_str());
		for(int j=0;j<count;j++){
			int x=rand()%upperLimit;
			f2<<x<<" ";
			actual_sum+=x;
		}
		f2.close();
		
	}
	cout<<nfileb<<" b_x files created succesfully"<<endl;
	cout<<"Sum of All random elements in all files is "<<actual_sum<<endl;
	return actual_sum;

}




int main(int argc, char* argv[]){
	int np[2];
	pipe(np);
	int ans;
	int f1=atoi(argv[1]);
	int f2=atoi(argv[2]);
	int num=atoi(argv[3]);
	int t1=atoi(argv[4]);
	int t2=atoi(argv[5]);
	int actual_sum=0;
	actual_sum=createFiles(f1,f2,num,actual_sum);
	pid_t pid=fork();
	if(pid==0){
		//Child Process
		int threadSum[t2];
		spawn(2, f2, t2, threadSum, num);
		int z=0;
		for(int i=0;i<t2;i++){
			z+=threadSum[i];
		}
		ans=z;
		close(np[0]);
		write(np[1], &ans, sizeof(ans));
		close(np[1]);

	}
	else{
		//Parent Process
		int threadSum[t1];
		spawn(1,f1,t1,threadSum, num);
		wait(NULL);
		close(np[1]);
		read(np[0], &ans, sizeof(ans));
		close(np[0]);
		for(int i=0;i<t1;i++){
			ans+=threadSum[i];
		}
		cout<<" Total sum across the both thread is "<<ans<<endl;
		if(actual_sum==ans){
			cout<<"Hurreyyyyyyyy!!! My solution is working correctly"<<endl;
		}
		else{
			cout<<"Sad Life : ( : ("<<endl;
		}




	}
	




}