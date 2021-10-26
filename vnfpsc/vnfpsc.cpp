//max sig(vixi)
//given sig(wixi<=W)
//and xi<=1,xi>=0
#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>
#include <ilconcert/iloexpression.h>

ILOSTLBEGIN
class flow{
public:
	int s,d;
	int bw,sc;
	int max_delay;

};
class Schain{
public:
	int vnfsize;
	vector <int> vnf;
	map <int,int> vnfmap;
};

int
main (int argc, char **argv){
	IloEnv env;
	try {
		int B,V,Esize,NF,nof,tosc,asize;
		float alpha;
		cin>>alpha;
		cin>>V>>B>>Esize>>NF>>asize>>nof>>tosc;
		vector<pair<int,int> >E(Esize);//n-r
		vector<vector<pair<int,int> > >Enei(V);
		vector<int>bwl(Esize);
		vector<int> a(asize);
		vector<flow>flow(nof);
		int nos=tosc;
		vector<Schain>schain(tosc);
		vector<vector<int> >M(NF,vector<int>(V));
		vector<int>Cu(V);
		vector<int>Ci(NF);
		vector<int>delta(NF);
		vector<int>delay(E.size());


		for(int i=0;i<Esize/2;i++){
			cin>>E[i].first>>E[i].second>>bwl[i]>>delay[i];
			Enei[E[i].first].push_back(make_pair(E[i].second,i));
			E[Esize-1-i]=make_pair(E[i].second,E[i].first);//remember
			Enei[E[i].second].push_back(make_pair(E[i].first,Esize-1-i));
			bwl[Esize-1-i]=bwl[i];
			delay[Esize-1-i]=delay[i];
		}
		for(int i=0;i<V;i++){
				cin>>Cu[i];
		}
		for(int i=0;i<NF;i++){
			cin>>Ci[i]>>delta[i];
		}
		for(int i=0;i<asize;i++){
			cin>>a[i];
		}
//		cout<<"here"<<endl;
		for(int i=0;i<NF;i++){
			for(int u=0;u<V;u++){
//				cout<<i<<","<<u<<endl;
				cin>>M[i][u];
			}
		}
		for(int f=0;f<nof;f++){
			cin>>flow[f].s;
			cin>>flow[f].d;
			cin>>flow[f].bw;
//			cout<<"yes1"<<endl;
			cin>>flow[f].sc;
//			cout<<"yes2"<<endl;
			cin>>flow[f].max_delay;
		}
		for(int i=0;i<tosc;i++){
			cin>>schain[i].vnfsize;
			schain[i].vnf.resize(schain[i].vnfsize);
			for(int j=0;j<schain[i].vnfsize;j++){
				cin>>schain[i].vnf[j];
				schain[i].vnfmap[schain[i].vnf[j]]=j;
			}
		}
/*
		vector<vector<vector<int> > >P;
		vector<vector<vector<int> > >active;
		vector<vector<vector<vector<int> > > >Z;
		vector<vector<vector<int> > >X;
		vector<vector<vector<int> > >support;
*/
		IloModel model(env);
cout<<"flag1"<<endl;
		vector<vector<vector<IloNumVar> > >P;
		P.reserve(nof);
		for(int i=0;i<nof;i++){
			vector<vector<IloNumVar> >temp1;
			temp1.reserve(schain[flow[i].sc].vnf.size()-1);
			for(int j=0;j<schain[flow[i].sc].vnf.size()-1;j++){
				vector<IloNumVar>temp0;
				temp0.reserve(E.size()+V);
				for(int k=0;k<E.size()+V;k++){
					IloNumVar tp(env,0,1,ILOINT);
					temp0.push_back(tp);
				}
				temp1.push_back(temp0);
			}
			P.push_back(temp1);
		}
cout<<"flag2"<<endl;
		vector<vector<vector<IloNumVar> > >active;
		active.reserve(NF);
		for(int i=0;i<NF;i++){
			vector<vector<IloNumVar> >temp1;
			temp1.reserve(V);
			for(int u=0;u<V;u++){
				vector<IloNumVar>temp0;
				temp0.reserve(M[i][u]);
				for(int k=0;k<M[i][u];k++){
					IloNumVar tp(env,0,1,ILOINT);
					temp0.push_back(tp);
				}
				temp1.push_back(temp0);
			}
			active.push_back(temp1);
		}
cout<<"flag3"<<endl;
		vector<vector<vector<IloNumVar> > >X;
		X.reserve(nof);
		for(int i=0;i<nof;i++){
			vector<vector<IloNumVar> >temp1;
			temp1.reserve(schain[flow[i].sc].vnf.size());
			for(int j=0;j<schain[flow[i].sc].vnf.size();j++){
				vector<IloNumVar>temp0;
				temp0.reserve(V);
				for(int k=0;k<V;k++){
					IloNumVar tp(env,0,1,ILOINT);
					temp0.push_back(tp);
				}
				temp1.push_back(temp0);
			}
			X.push_back(temp1);
		}
cout<<"flag4"<<endl;
		vector<vector<vector<IloNumVar> > >support;
		support.reserve(NF);
		for(int i=0;i<NF;i++){
			vector<vector<IloNumVar> >temp1;
			temp1.reserve(V);
			for(int u=0;u<V;u++){
				vector<IloNumVar>temp0;
				temp0.reserve(M[i][u]);
				for(int k=0;k<M[i][u];k++){
					IloNumVar tp(env,1,nos,ILOINT);
					temp0.push_back(tp);
				}
				temp1.push_back(temp0);
			}
			support.push_back(temp1);
		}
cout<<"flag5"<<endl;
		vector<vector<vector<vector<IloNumVar> > > >Z;
		Z.reserve(nof);
		for(int i=0;i<nof;i++){
			vector<vector<vector<IloNumVar> > >temp2;
			temp2.reserve(schain[flow[i].sc].vnf.size());
			for(int j=0;j<schain[flow[i].sc].vnf.size();j++){
				vector<vector<IloNumVar> >temp1;
				temp1.reserve(V);
				for(int u=0;u<V;u++){
					vector<IloNumVar>temp0;
					temp0.reserve(M[schain[flow[i].sc].vnf[j]][u]);
					for(int l=0;l<M[schain[flow[i].sc].vnf[j]][u];l++){
						IloNumVar tp(env,0,1,ILOINT);
						temp0.push_back(tp);
					}
					temp1.push_back(temp0);
				}
				temp2.push_back(temp1);
			}
			Z.push_back(temp2);
		}
cout<<"flag6"<<endl;
//obj-------------------------------------------------------------------------------------------------------
		IloNumExpr ineo(env,0);
		for(int f=0;f<nof;f++){
			for(int e1=0;e1<E.size();e1++){
				for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
					ineo+=P[f][e2][e1]*flow[f].bw*alpha/(B*E.size()/2);
				}
			}
		}
		for(int u=0;u<V;u++){
			for(int i=0;i<NF;i++){
				for(int j=0;j<M[i][u];j++){
					ineo+=active[i][u][j]*(1-alpha)/(NF*V);
				}
			}
		}
cout<<"flag0"<<endl;
//c01-------------------------------------------------------------------------------------------------------
for(int i=0;i<NF;i++){
	for(int j=0;j<V;j++){
		for(int k=0;k<M[i][j];k++){
			cout<<i<<j<<k<<" "<<active[i][j][k]<<endl;
		}
	}
}
		for(int u=0;u<V;u++){
			for(int i=0;i<NF;i++){
				for(int j=0;j<M[i][u];j++){
					IloNumExpr inec(env,0);
					for(int f=0;f<nof;f++){
cout<<u<<i<<j<<f<<endl;
						if(schain[flow[f].sc].vnfmap.find(i)!=schain[flow[f].sc].vnfmap.end()){
cout<<"i"<<endl;
cout<<Z[f][schain[flow[f].sc].vnfmap[i]][u][j]<<" nene"<<endl;
cout<<active[i][u][j]<<endl;
							inec += Z[f][schain[flow[f].sc].vnfmap[i]][u][j]-Z[f][schain[flow[f].sc].vnfmap[i]][u][j]*active[i][u][j];
						}
cout<<"j"<<endl;
					}
					model.add(inec <= 0);
					model.add(inec >= 0);
cout<<"-------------------------------------"<<inec<<endl;
				}
			}
		}
cout<<"flag1"<<endl;
//c02-------------------------------------------------------------------------------------------------------
		for(int f=0;f<nof;f++){
			for(int u=0;u<V;u++){
				for(int i=0;i<schain[flow[f].sc].vnf.size();i++){

					IloNumExpr inec(env,0);
					for(int j=0;j<M[schain[flow[f].sc].vnf[i]][u];j++){
						inec+= Z[f][i][u][j];
					}
					model.add(inec <= X[f][i][u]);
					model.add(inec >= X[f][i][u]);
				}
			}
		}

cout<<"flag2"<<endl;
//c03-------------------------------------------------------------------------------------------------------
		for(int f=0;f<nof;f++){
			for(int i=0;i<schain[flow[f].sc].vnf.size();i++){

				IloNumExpr inec(env,0);
				for(int u=0;u<V;u++){
					inec += X[f][i][u];
				}
				model.add(inec<=1);
				model.add(inec>=1);
			}
		}

cout<<"flag3"<<endl;
//c04-------------------------------------------------------------------------------------------------------
		for(int f=0;f<nof;f++){
			for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){

				IloNumExpr inec(env,0);
				for(int e1=0;e1<E.size();e1++){
						inec+=P[f][e2][e1]*X[f][e2][E[e1].first];
				}
				for(int e1=E.size();e1<E.size()+V;e1++){
						inec+=P[f][e2][e1]*X[f][e2][e1-E.size()];
				}
				if(e2==schain[flow[f].sc].vnfsize-2){
					for(int e1=0;e1<E.size();e1++){
							inec+=P[f][e2][e1]*X[f][e2+1][E[e1].second];
					}
					for(int e1=E.size();e1<E.size()+V;e1++){
							inec+=P[f][e2][e1]*X[f][e2+1][e1-E.size()];
					}
				}
				model.add(inec<=1);
				model.add(inec>=1);
			}
		}

cout<<"flag4"<<endl;
//c05-------------------------------------------------------------------------------------------------------
/*		for(int i=0;i<a.size();i++){
			for(int u=0;u<V;u++){
				for(int j=1;j<M[a[i]][u];j++){
					for(int f=0;f<nof;f++){
						if(schain[flow[f].sc].vnfmap.find(a[i])!=schain[flow[f].sc].vnfmap.end()){
							IloNumExpr inec(env,0);
							inec+=Z[f][schain[flow[f].sc].vnfmap[a[i]]][u][j]*flow[f].sc;
							inec-=Z[f][schain[flow[f].sc].vnfmap[a[i]]][u][j]*support[a[i]][u][j];
							model.add(inec<=0);
							model.add(inec>=0);
						}
					}
				}
			}
		}
cout<<"flag5"<<endl;
*/
//06-------------------------------------------------------------------------------------------------------
		for(int f=0;f<nof;f++){
			for(int u=0;u<V&&u!=flow[f].s&&u!=flow[f].d;u++){

				IloNumExpr inec(env,0);
				for(int v=0;v<Enei[u].size();v++){
					for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
						inec+=P[f][e2][Enei[u][v].second]-P[f][e2][Esize-1-Enei[u][v].second];
					}
				}
				model.add(inec<=0);
				model.add(inec>=0);
			}
		}
cout<<"flag6"<<endl;

//c07-------------------------------------------------------------------------------------------------------
		for(int f=0;f<nof;f++){
			IloNumExpr inec(env,0);
			for(int v=0;v<Enei[flow[f].s].size();v++){
				for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
					inec+=P[f][e2][Enei[flow[f].s][v].second]-P[f][e2][Esize-1-Enei[flow[f].s][v].second];
				}
			}
			model.add(inec<=1);
			model.add(inec>=1);
		}

//c08-------------------------------------------------------------------------------------------------------
		for(int f=0;f<nof;f++){
			IloNumExpr inec(env,0);
			for(int v=0;v<Enei[flow[f].d].size();v++){
				for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
					inec+=P[f][e2][Enei[flow[f].d][v].second]-P[f][e2][Esize-1-Enei[flow[f].d][v].second];
				}
			}
			model.add(inec<=-1);
			model.add(inec>=-1);
		}

//c09-------------------------------------------------------------------------------------------------------
		for(int e1=0;e1<E.size();e1++){

			IloNumExpr inec(env,0);
			for(int f=0;f<flow.size();f++){
				for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
					inec+=P[f][e2][e1]*flow[f].bw;
				}
			}
			model.add(inec<=bwl[e1]);//circle back
		}

//c10-------------------------------------------------------------------------------------------------------
		for(int u=0;u<V;u++){

			IloNumExpr inec(env,0);
			for(int i=0;i<NF;i++){
				for(int j=0;j<M[i][u];j++){
					inec+= active[i][u][j]*Ci[i];
				}
			}
			model.add(inec<=Cu[u]);
		}

//c11-------------------------------------------------------------------------------------------------------
		for(int u=0;u<V;u++){
			for(int i=0;i<NF;i++){
				for(int j=0;j<M[i][u];j++){

					IloNumExpr inec(env,0);
					for(int f=0;f<nof;f++){
						if(schain[flow[f].sc].vnfmap.find(i)!=schain[flow[f].sc].vnfmap.end()){
							inec+=Z[f][schain[flow[f].sc].vnfmap[i]][u][j]*delta[i]*flow[f].bw;
						}
					}
					model.add(inec<=Ci[u]);
				}
			}
		}

//c12-------------------------------------------------------------------------------------------------------
		for(int f=0;f<nof;f++){

			IloNumExpr inec(env,0);
			for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
				for(int e1=0;e1<E.size();e1++){
					inec+=P[f][e2][e1]*delay[e1];
				}
			}
			model.add(inec<=flow[f].max_delay);
		}

//c13-------------------------------------------------------------------------------------------------------
/*		for(int i=;i<;i++){
			for(int j=;j<j++){
				for(int k=;k<;k++){
					for(int l=;l<;l++){

IloNumExpr inec(env,0);
						for(int q=;q<;q++){
							
						}
model.add(inec);
					}
				}
			}
		}
*/

//c14-------------------------------------------------------------------------------------------------------
		for(int f=0;f<nof;f++){
			for(int u=0;u<V;u++){
				for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
					for(int v=0;v<Enei[u].size();v++){

						IloNumExpr inec(env,0);
						for(int w=0;w<Enei[u].size();w++){
							inec+=P[f][e2][Enei[u][v].second]*P[f][e2][E.size()-1-Enei[u][w].second];
						}
						inec+=P[f][e2][Enei[u][v].second]*X[f][e2][u]-P[f][e2][Enei[u][v].second];
						model.add(inec<=0);
						model.add(inec>=0);
					}
				}
			}
		}


//c15-------------------------------------------------------------------------------------------------------
		for(int f=0;f<nof;f++){
			for(int u=0;u<V;u++){
				for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
					for(int v=0;v<Enei[u].size();v++){

						IloNumExpr inec(env,0);
						for(int w=0;w<Enei[u].size();w++){
							inec+=P[f][e2][E.size()-1-Enei[u][v].second]*P[f][e2][Enei[u][w].second];
						}
						inec+=P[f][e2][E.size()-1-Enei[u][v].second]*X[f][e2+1][u]-P[f][e2][E.size()-1-Enei[u][v].second];
						model.add(inec<=0);
						model.add(inec>=0);
					}
				}
			}
		}
//c16-------------------------------------------------------------------------------------------------------

/*
		for(int f=0;f<nof;f++){
			IloNumExpr inec(env,0);
			inec+=X[f][0][flow[f].s]+X[f][schain[flow[f].sc].vnf.size()-1][flow[f].d];
			model.add(inec>=2);
		}*/

/*

		for(int f=0;f<nof;f++){
			for(int u=0;u<V;u++){
				for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){

					IloNumExpr inec(env,0);
					for(int w=0;w<Enei[u].size();w++){
						inec+=P[f][e2][Enei[u][v].second]*P[f][e2][E.size()-1-Enei[u][w].second];
					}
					inec+=P[f][e2][Enei[u][v].second]*X[f][i][u]-P[f][e2][Enei[u][v].second];
					model.add(inec<=0);
					model.add(inec>=0);
				}
			}
		}

*/



		model.add(IloMinimize(env, ineo));
		IloCplex cplex(model);
cout<<"ihjgkhkgkgygiy0"<<endl;
cplex.setParam(IloCplex::Param::OptimalityTarget,IloCplex::OptimalityOptimalGlobal);
cout<<"ihjgkhkgkgygiy1"<<endl;
		if ( !cplex.solve() ) {
cout<<"ihjgkhkgkgygiy2"<<endl;
			env.error() << "Failed to optimize LP." << endl;
cout<<"ihjgkhkgkgygiy3"<<endl;
			throw(-1);
		}
		IloNumArray vals(env);
		env.out() << "Solution status = " << cplex.getStatus() << endl;
		env.out() << "Solution value = " << cplex.getObjValue() << endl;
//		cplex.getValues(vals, vars);
//		env.out() << "Values = " << vals << endl;
		int temp;
		cout<<"flow:"<<endl;
		for(int f=0;f<nof;f++){
			cout<<"f"<<f<<":"<<endl;
			for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
				for(int e1=0;e1<E.size();e1++){
					temp=cplex.getValue(P[f][e2][e1]);
					if(temp==1){
						cout<<"("<<E[e1].first<<","<<E[e1].second<<")	";
						cout<<"("<<schain[flow[f].sc].vnf[e2]<<","<<schain[flow[f].sc].vnf[e2+1]<<")	"<<endl;;
					}
				}
				for(int e1=E.size();e1<E.size()+V;e1++){
					temp=cplex.getValue(P[f][e2][e1]);
					if(temp==1){
						cout<<"("<<e1-E.size()<<")	";
						cout<<"("<<schain[flow[f].sc].vnf[e2]<<","<<schain[flow[f].sc].vnf[e2+1]<<")	"<<endl;
					}
				}
			}
		}
		cout<<"Active:"<<endl;
		for(int i=0;i<NF;i++){
			for(int u=0;u<V;u++){
				for(int j=0;j<M[i][u];j++){
					temp=cplex.getValue(active[i][u][j]);
					if(temp==1){
						cout<<"("<<i<<","<<u<<","<<j<<")"<<endl;
					}
				}
			}
		}
		/*
		for(int f=0;f<nof;f++){
			for(int i=0;<;++){
				for(int =0;<;++){
					for(int =0;<;++){
						temp=cplex.getValue(Z[f][i][u][j]);

					}
				}
			}
		}
		for(int =0;f<;++){
			for(int =0;<;++){
				for(int =0;<;++){
					temp=cplex.getValue(X[f][i][u]);
				}
			}
		}
		for(int =0;f<;++){
			for(int =0;<;++){
				for(int =0;<;++){
					temp=cplex.getValue(support[a[i]][u][j]);
					
				}
			}
		}

*/


	}
	catch (IloException& e) {
		cerr << "Concert exception caught: " << e << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
	}
	env.end();
	return 0;
}