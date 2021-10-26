//max sig(vixi)
//given sig(wixi<=W)
//and xi<=1,xi>=0
#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>
#include <ilconcert/iloexpression.h>
ILOSTLBEGIN
int
main (int argc, char **argv){
	IloEnv env;
	try {
		IloModel model(env);
		IloNumVarArray vars(env);
		int n,W;
		cin>>n;
		cin>>W;
		vector<int> value(n);
		vector<int> weight(n);
		IloNumExpr ine_obj(env,0),ine_cns(env,0);
		for(int i=0;i<n;i++){
			cin>>weight[i];
			cin>>value[i];
			vars.add(IloNumVar(env, 0, 1, ILOINT));
			ine_obj += value[i]*vars[i];
			ine_cns += weight[i]*vars[i];
		}
		model.add(IloMaximize(env, ine_obj));
		model.add(ine_cns <= W);
		IloCplex cplex(model);
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}
		IloNumArray vals(env);
		env.out() << "Solution status = " << cplex.getStatus() << endl;
		env.out() << "Solution value = " << cplex.getObjValue() << endl;
		cplex.getValues(vals, vars);
		env.out() << "Values = " << vals << endl;
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
