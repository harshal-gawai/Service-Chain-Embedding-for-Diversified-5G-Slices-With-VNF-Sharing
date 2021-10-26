//max sig(vixi)
//given sig(wixi<=W)
//and xi<=1,xi>=0
#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>
#include <ilconcert/iloexpression.h>

ILOSTLBEGIN

//A structure to represent a flow.
class flow{
public:
	int s,d;
	int bw,sc;
	int max_delay;

};

//A structure to store service chain's vnfs and their placements
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
		/*
		B = avg. bandwidth
		V = num.of vertices
		Esize = num.of directed edges
		NF = num.of vnfs
		nof = num.of flows
		tosc = types of service chains
		asize = num.of non sharable VNFs
		*/ 
		
		int B,V,Esize,NF,nof,tosc,asize;
		float alpha;

		//Taking input
		cin>>alpha;
		cin>>V>>B>>Esize>>NF>>asize>>nof>>tosc;

		/*
		E = data structure to store directed edges.
		Enei = data structure  to store adjacent vertices of any vertex and the corresponding edge number.
		bwl = data structure to store band width limit for edges.
		a = data structure to store non-sharable VNFs
		flow = data structure to store the flows.
		schain = data structure to store VNFs belongs to a particular slice.
		M = data structure to store maximum num.of VNF instances of a specific VNF can be placed in a particular node.
		Cu = data structure to store computational resource capacity of a every node.
		Ci = data structure  to store computational resource required by every VNFs.
		delta = data structure to store per-unit computational resource requirement of every VNF.
		delay = data structure to store the delay between edges.
		*/

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

		//Scanning for edges and storing them in corresponding data structure.
		for(int i=0;i<Esize/2;i++){
			cin>>E[i].first>>E[i].second>>bwl[i]>>delay[i];
			Enei[E[i].first].push_back(make_pair(E[i].second,i));
			E[Esize-1-i]=make_pair(E[i].second,E[i].first);//remember
			Enei[E[i].second].push_back(make_pair(E[i].first,Esize-1-i));
			bwl[Esize-1-i]=bwl[i];
			delay[Esize-1-i]=delay[i];
		}

		//scanning for computational resource capacity of a every node.
		for(int i=0;i<V;i++){
				cin>>Cu[i];
		}

		//scanning for computational resource, per-unit computational resource required by every VNF
		for(int i=0;i<NF;i++){
			cin>>Ci[i]>>delta[i];
		}

		//scanning for non-sharable VNFs
		for(int i=0;i<asize;i++){
			cin>>a[i];
		}

		//Scanning for maximum num.of VNF instances of a specific VNF can be placed in a particular node.
		for(int i=0;i<NF;i++){
			for(int u=0;u<V;u++){
				cin>>M[i][u];
			}
		}

		//Scanning for every flow attributes.
		for(int f=0;f<nof;f++){
			cin>>flow[f].s;
			cin>>flow[f].d;
			cin>>flow[f].bw;
			cin>>flow[f].sc;
			cin>>flow[f].max_delay;
		}

		//Scanning for VNFs belonging to a particular slice.
		for(int i=0;i<tosc;i++){
			cin>>schain[i].vnfsize;
			schain[i].vnf.resize(schain[i].vnfsize);
			for(int j=0;j<schain[i].vnfsize;j++){
				cin>>schain[i].vnf[j];
				schain[i].vnfmap[schain[i].vnf[j]]=j;
			}
		}

		IloModel model(env);

		//P is 3 dimensional data structure for every flow f, every physical edge e1 and for every virtual edge in that flow.
		//P_f_e2_e1 is a binary variable that denotes if physical edge e1 is serving virtual edge e2 in flow f.
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

		//active is 3 dimensional data structure for every node u, for every VNF i and for every jth instance of i.
		//It denotes whether VNF i’s jth instance is active or not in physical node u.
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

		//X is a 3 dimensional data structure for every node u, for every VNF i and for every flow f.
		//It denotes whether VNF i is present in physical node u or not for flow f.
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

		//Z is a 4 dimensional data structure for every node u, for every VNF i's jth instance and for every flow f.
		//It represents whether VNF i’s jth instance is active or not in physical node u for flow f
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

//objective function-------------------------------------------------------------------------------------------------------
		//It calculates the total amount of bandwidth and total computational resources required in a given physical network.
		//Later we will minimize this expression using ILP.
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

//constraint01-------------------------------------------------------------------------------------------------------
		//This constraint is to make sure that if a flow f passes through physical node u and VNF i’s jth instance, then that VNF will be considered as active VNF.
		for(int u=0;u<V;u++){
			for(int i=0;i<NF;i++){
				for(int j=0;j<M[i][u];j++){
					IloNumExpr inec(env,0);
					for(int f=0;f<nof;f++){
						if(schain[flow[f].sc].vnfmap.find(i)!=schain[flow[f].sc].vnfmap.end()){
							inec += Z[f][schain[flow[f].sc].vnfmap[i]][u][j]-Z[f][schain[flow[f].sc].vnfmap[i]][u][j]*active[i][u][j];
						}
					}
					model.add(inec <= 0);
					model.add(inec >= 0);
				}
			}
		}

//constraint02-------------------------------------------------------------------------------------------------------
		//This constraint is to make sure that the sum of all instances of VNFs i in a node u for flow f is equal to X_f,i,u.
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


//constraint03-------------------------------------------------------------------------------------------------------
		//This constraint is to make sure that any VNF in the flow f should be present in a single physical node only.
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


//constraint04-------------------------------------------------------------------------------------------------------
		//This constraint is to make sure that if a VNF i is present in a flow f , there should be one outgoing edge and one incoming edge in its path.
		for(int f=0;f<nof;f++){
			for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
				for(int u=0;u<V;u++){
					IloNumExpr inec(env,0);
					for(int v=0;v<Enei[u].size();v++){
						inec+=P[f][e2][Enei[u][v].second]*X[f][e2][u];
					}
					inec+=P[f][e2][Esize+u]*X[f][e2][u]-X[f][e2][u];
					model.add(inec<=0);
					model.add(inec>=0);
				}
			}
		}

		for(int f=0;f<nof;f++){
			for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
				for(int u=0;u<V;u++){
					IloNumExpr inec(env,0);
					for(int v=0;v<Enei[u].size();v++){
						inec+=P[f][e2][Esize-1-Enei[u][v].second]*X[f][e2+1][u];
					}
					inec+=P[f][e2][Esize+u]*X[f][e2+1][u]-X[f][e2+1][u];
					model.add(inec<=0);
					model.add(inec>=0);
				}
			}   
		}

//constraint05-------------------------------------------------------------------------------------------------------
		//5,6,7 constrain on the number of edges entering and leaving a physical node u for flow f.
		//constarint 5:For every flow f, the number of edges coming into a node should be equal to number of edges goint out of the nodes except for source and destination.
		//constraint 6:For source,latter is more by one.
		//constraint 7:For destination, former is more by one.
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

//constraint06-------------------------------------------------------------------------------------------------------
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

//constraint07-------------------------------------------------------------------------------------------------------
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

//constraint08-------------------------------------------------------------------------------------------------------
		//This constraint is to make sure that the residual bandwidth on the edge e1 must be sufficient to serve all the flows using it on the path.
		for(int e1=0;e1<E.size();e1++){

			IloNumExpr inec(env,0);
			for(int f=0;f<flow.size();f++){
				for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
					inec+=P[f][e2][e1]*flow[f].bw;
				}
			}
			model.add(inec<=bwl[e1]);
		}

//constraint09-------------------------------------------------------------------------------------------------------
		//This constraint ios to make sure that the computational resource of any node u must be sufficient for all the VNFs embedded in node u to process all the traffic flows traversing node u.
		for(int u=0;u<V;u++){

			IloNumExpr inec(env,0);
			for(int i=0;i<NF;i++){
				for(int j=0;j<M[i][u];j++){
					inec+= active[i][u][j]*Ci[i];
				}
			}
			model.add(inec<=Cu[u]);
		}

//constraint10-------------------------------------------------------------------------------------------------------
		//This constraint is to make sure that the maximum amount of data that can be processed by VNF i jth instance over all flows per unit time is atmost C_i.
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

//constraint11-------------------------------------------------------------------------------------------------------
		//This constraint is to make sure that summation of all delays in every physical edge that serves a virtual edge should be less than flow f’s max delay.
		for(int f=0;f<nof;f++){

			IloNumExpr inec(env,0);
			for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
				for(int e1=0;e1<E.size();e1++){
					inec+=P[f][e2][e1]*delay[e1];
				}
			}
			model.add(inec<=flow[f].max_delay);
		}

//constraint12-------------------------------------------------------------------------------------------------------
		//12,13,14 VNF chaining constraints.
		//constraint 12: is to make sure that for every flow f , and for any physical edge serving a virtual edge, the physical edge should be met with another incoming physical edge serving the same virtual edge or virtual edge should originate from the corresponding physical edge.
		//constraint 13: is to make sure that for every flow f , and for any physical edge serving a virtual edge, the physcial edge should be met with another outgoing physical edge serving the same virtual edge or virtual edge should end at the corresponding physical edge.
		//constraint 14: is to make sure that for every flow f , if virtual edge resides in a physical node, then there should be only single physical edge serving the virtual edge.
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

		for(int f=0;f<nof;f++){
			for(int e2=0;e2<schain[flow[f].sc].vnfsize-1;e2++){
				IloNumExpr inec(env,0);
				for(int k=0;k<P[f][e2].size();k++){
					inec+=P[f][e2][k];
				}
				model.add(inec>=1);
			}
		}

//constraint13-------------------------------------------------------------------------------------------------------
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
//constraint14-------------------------------------------------------------------------------------------------------

		for(int f=0;f<nof;f++){
			for(int u=0;u<V;u++){
				for(int e2=0;e2<schain[flow[f].sc].vnf.size()-1;e2++){
						IloNumExpr inec(env,0);
						inec+=P[f][e2][E.size()+u]*X[f][e2][u]+P[f][e2][E.size()+u]*X[f][e2+1][u]-2*P[f][e2][E.size()+u];
						model.add(inec<=0);
						model.add(inec>=0);
				}
			}
		}

		for(int f=0;f<nof;f++){
			IloNumExpr inec(env,0);
			inec+=X[f][0][flow[f].s]+X[f][schain[flow[f].sc].vnf.size()-1][flow[f].d];
			model.add(inec>=2);
		}


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
		cout<<"X:"<<endl;
		for(int f=0;f<nof;f++){
			for(int i=0;i<schain[flow[f].sc].vnfsize;i++){
				for(int u=0;u<V;u++){
					temp=cplex.getValue(X[f][i][u]);
					if(temp==1){
						cout<<f<<","<<schain[flow[f].sc].vnf[i]<<","<<u<<endl;
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