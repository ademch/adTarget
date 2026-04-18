
var succ_5_11 = [ 
	[ 2-1, 6-1 ],
	[ 3-1, 4-1, 5-1 ], 
	[ 1-1 ], 
	[ 3-1, 5-1 ], 
	[ 1-1 ], 			// 5-1
	[ 7-1, 10-1, 11-1 ], 
	[ 5-1, 8-1, 9-1 ], 
	[ 5-1 ], 
	[ 8-1 ], 
	[ 11-1 ], 			// 10-1 
	[ ], 
	[ 10-1, 11-1 ], 
	[ 9-1, 11-1, 15-1 ], 
	[ 13-1 ], 
	[ 14-1 ] 			// 15-1
]; 

var succ_5_11_wrong = [ 
	[ 2, 6 ], 
	[ 3, 4, 5 ], 
	[ 1 ], 
	[ 3, 5 ], 
	[ 1 ], 			// 5
	[ 7, 10, 11 ], 
	[ 5, 8, 9 ], 
	[ 5 ], 
	[ 8 ], 
	[ 11 ], 			// 10
	[ ], 
	[ 10, 11 ], 
	[ 9, 11, 15 ], 
	[ 13 ], 
	[ 14 ] 			// 15
]; 

var succ_5_14 = [ 
	[ 2-1, 10-1 ], 
	[ 3-1, 5-1, 7-1 ], 
	[ 4-1 ], 
	[ 2-1 ], 
	[ 4-1, 6-1 ], 		// 5-1
	[  ], 
	[ 8-1 ], 
	[ 6-1, 9-1 ], 
	[ 7-1 ], 
	[ 7-1, 11-1, 15-1 ],  	// 10-1
	[ 12-1 ], 
	[ 13-1 ], 
	[ 9-1, 10-1, 14-1 ], 
	[ 11-1 ], 
	[ 16-1 ], 			// 15-1
	[ 1-1, 17-1 ], 
	[ 14-1, 15-1 ] 
]; 

//var trmx = Tremaux(0,succ_5_11); 
var trmx = CalculeComposantesFortementConnexe(succ_5_11);
//var trmx = CalculeComposantesFortementConnexe(succ_5_11_wrong);
//var trmx = CalculeComposantesFortementConnexe(succ_5_14);

////////////////////////////////////////////////////////////////////////////////

// Plus court chemin:
// Dijkstra's Shortest Paths
// Bellman-Ford Shortest Paths (allows negative weights)
// Roy–Floyd–Warshall algorithm (directed graph, nice and simple)
// Johnson's All-Pairs Shortest Paths (complex)

// Roy–Floyd–Warshall:
// k=0 :  shortestPath(i,j,0) = edgeCost(i,j).
// k<n :  shortestPath(i,j,k) = min{shortestPath(i,j,k - 1),shortestPath(i,k,k - 1) + shortestPath(k,j,k - 1)},
// Applications: transitive closure, logical evaluation, regexp, real matrix inversion

// Arbre couvrant:
// Kruskal's Minimum Spanning Tree
// Prim's Minimum Spanning Tree

// Cuthill Mckee Ordering (application to sparse matrices)

////////////////////////////////////////////////////////////////////////////////

// x : node from which we browse the graph 
// succ : graph described by the vector of the list of successors for each node 
function Tremaux(x, succ) 
{ 
	var n = succ.length; 	
	var prec = new Array(n); 
	var order = new Array(n);   // renumerotation des noeuds tq. on a une arborescence prefixe 
	var attach = new Array(n); 
	
	omega = -1; 
	for (var i=0; i<n; i++) 
		prec[i] = order[i] = attach[i] = omega; 
	prec[x] = x; 
	order[x] = 0; 
	
	var nodes = [ ] 
	TremauxRecurse(x, succ, prec, order, attach, nodes); 
		
	for (var i=0; i<n; i++) 
		WScript.echo("prec["+i+"]="+prec[i]+" @ "+order[i]+" Att "+attach[i]); 
	//for (var i=0; i<nodes.length; i++) 
	//	WScript.echo("nodes["+i+"]="+nodes[i]); 
	
	return { prec: prec }; 
} 

function TremauxRecurse (
	u, 		///< sommet racine
	succ, 
	// outputs
	prec,		///< [nbSommets]		predecesseus des sommets 
	order, 	///< [nbSommets]		ordre de parcours des sommets (omega si le sommet n'est pas atteignable)
	attach, 	///< [nbSommets]		id du point d'attachement
	nodes 	///< [nbSommetArbo]		id des sommets classe selon l'ordre de parcours de l'arbre
	) 
{ 
	order[u] = nodes.length; 
	nodes.push(u); 
	
	attach[u] = u;
	var attach_min = u;
	
	var nbSucc = succ[u].length; 
	for (var k=0; k<nbSucc; k++) { 
		var v = succ[u][k] 
	
		if (prec[v] == omega)		// check if tree browse already covered v : we can test prec[v] or order[v] or attach[v]
		{ 
			//WScript.echo("TremauxRecurse adds prec["+v+"]="+u); 
			prec[v] 	= u; 
			TremauxRecurse(v, succ, prec, order, attach, nodes); 
			attach_min = Math.min(attach_min, attach[v]);
		} else {
			attach_min = Math.min(attach_min, v);
		}
	} 

	attach[u] = attach_min;
} 

////////////////////////////////////////////////////////////////////////////////

function CalculeComposantesFortementConnexe (
	succ 		///< [nbSommets] liste des successeurs pour chaque sommet 
)
{	
	var n = succ.length;			///< nbSommets
	var idCnx = new Array(n); 		///< [nbSommets] id de la composante connexe de chaque sommet
	var attach = new Array(n); 		///< [nbSommets] pt d'attachement de chaque sommet
	var order = new Array(n);   		///< [nbSommets] renumerotation des noeuds tq. on a une arborescence prefixe 
	
	omega = -1;
	for (var i=0; i<n; i++)
		idCnx[i] = attach[i] = omega; 
	
	var nodes = [ ] 
	var CnxCmpts = []; 			///< [nbConnexeCmpts] ids constituant chaque composante connexe 
	for (var u=0; u<n; u++) {
		if (idCnx[u]==omega && attach[u]==omega) {
			for (var i=0; i<n; i++) 
				order[i] = omega;
			NbCnxCmpts = PointAttacheRecurse(u, succ, order, nodes, attach, idCnx, CnxCmpts); 
		}
	}
	
	DotDisplay (succ, attach, idCnx, CnxCmpts);
	return { CnxCmpts: CnxCmpts, attach: attach, idCnx: idCnx };
}

function PointAttacheRecurse(
	u, 			///< sommet racine
	succ, 
	// outputs
	order, 		///< [nbSommets]		ordre de parcours des sommets (omega si le sommet n'est pas atteignable)
	nodes, 		///< [nbSommetArbo]		id des sommets classe selon l'ordre de parcours de l'arbre
	attach, 		///< [nbSommets]		id du point d'attachement
	idCnx,		///< [nbSommets]		id de composante connexe
	CnxCmpts 		///< [nbConnexeCmpts]	ids constituant chaque composante connexe 
	)
{ 
	var n = succ.length; 

	order[u] = nodes.length; 
	nodes.push(u); 

	attach[u] = u;      // Recherche du point d'attache de u (any numbering ??)
	//WScript.echo("PointAttacheRecurse (a): attach[u="+u+"]="+attach[u]+" := u"); 
	
	var nbSucc = succ[u].length; 
	for (var k=0; k<nbSucc; k++) { 
		var v = succ[u][k] 
		
		var pre_attach_u = attach[u];
		if (attach[v] == omega) { 	// check if tree browse already covered v : we can test prec[v] or order[v] or attach[v]
			if (idCnx[v] == omega)	// check that this is not in an already know convex component
			{
				PointAttacheRecurse(v, succ, order, nodes, attach, idCnx, CnxCmpts); 
				attach[u] = Math.min(attach[u], attach[v]);
				//WScript.echo("PointAttacheRecurse (b): attach[u="+u+"]="+attach[u]+" := min(attach[u]="+pre_attach_u+",attach[v="+v+"]="+attach[v]+")"); 
			}
		} else if (idCnx[v] == omega) {
			// v does not already belong to a connex component
			attach[u] = Math.min(attach[u], v); 
			//WScript.echo("PointAttacheRecurse (c): attach[u="+u+"]="+attach[u]+" := min(attach[u]="+pre_attach_u+",v="+v+")"); 
		} else {
			//WScript.echo("PointAttacheRecurse (d): attach[u="+u+"]="+attach[u]+" unchanged."); 
		}
	}
	
	if (u == attach[u]) { 
		CnxCmpts.push([]); 		//  ajoute une nouvelle composante connexe
		//WScript.echo("PointAttacheRecurse (e): nouvelle composante connexe "+(CnxCmpts.length-1)+" from u="+u+"."); 
		Supprimer(u, succ, order, idCnx, CnxCmpts); 
		//WScript.echo("PointAttacheRecurse (e): incluant les sommets { "+CnxCmpts[CnxCmpts.length-1]+" }."); 
	}
} 

// La suppression d'un sommet s'effectue
function Supprimer(
	u, 			///< racine
	succ, 		///< graphe
	order, 		///< [nbSommets]		ordre de parcours des sommets (omega si le sommet n'est pas atteignable)
	idCnx,		///< [nbSommets]		id de composante connexe
	CnxCmpts 		///< les composantes connexes (nous sommes en train de populer la derniere composante trouvee)
) 
{
	var nbCnxCmpts = CnxCmpts.length;
	idCnx[u] = nbCnxCmpts-1; 
	CnxCmpts[nbCnxCmpts-1].push(u); 
	//WScript.echo("SupprimerCnxCmpt (a): ajoute u="+u+" to CnxCmpt="+(CnxCmpts.length-1)+"."); 
		
	var nbSucc = succ[u].length; 
	for (var k=0; k<nbSucc; k++) { 
		var v = succ[u][k] 
		
		if ( (order[v] > order[u]) && (idCnx[v] == omega) ) 
			Supprimer(v, succ, order, idCnx, CnxCmpts); 
	} 
} 

////////////////////////////////////////////////////////////////////////////////

function DotDisplay (
	succ, attach, idCnx, CnxCmpts
)
{
	var n = succ.length;			///< nbSommets
	var txt = "digraph G {\n";
	for (var i=0; i<n; i++) {
		txt += "	"+i+" [label=\""+i+" at="+attach[i]+"\"];\n";
		for (var j=0; j<succ[i].length; j++) 
			txt += "	"+i+" -> "+succ[i][j]+";\n"; 
		//WScript.echo("attch/idCnx["+i+"]="+attach[i]+"/"+idCnx[i]); 
	}
	var nbCnxCmpts = CnxCmpts.length;
	for (var i=0; i<nbCnxCmpts; i++) {
		WScript.echo("CnxCmpts["+i+"] = { "+CnxCmpts[i]+" }"); 
		if (true) {
			txt += "	subgraph cluster_"+i+" {\n";
			txt += "		"+CnxCmpts[i]+";\n"
			txt += "	}\n";
		} 
	}
	txt += "}";
	
	var dotExe = "\"C:\\Program Files\\Graphviz 2.21\\bin\\dot\"";
	var dotArg = " -v -T png -O ";
	var dotFile = "..\\..\\vision\\dot\\connex.gv";
	//WScript.echo(txt+"\n");
	WScript.echo("cls && "+dotExe+" "+dotArg+" "+dotFile+" && "+dotFile+".png");
	toFile(dotFile, txt)
	
	return txt;
}

function toFile(filename, txt) {
	var fso = new ActiveXObject("Scripting.FileSystemObject"); 
	var ForWriting = 2;
	var file = fso.OpenTextFile(filename, ForWriting, true);
	file.Write(txt);
}
function fromFile(filename) {
	var fso = new ActiveXObject("Scripting.FileSystemObject"); 
	var ForReading = 1;
	var file = fso.OpenTextFile(filename, ForReading, false);
	var txt = file.ReadAll();
	return txt;
}
