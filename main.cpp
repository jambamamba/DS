#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ds.h"
#include "../common/error_enum.h"

#include <time.h>

//#define TEST_SORTED_LIST_DS
#define TEST_AVL_TREE
//#define TEST_TREE_DS

#if (defined(TEST_SORTED_LIST_DS) || defined(TEST_TREE_DS))

FILE *fp=0;

class MYOBJ : public OBJECT
{
public:
	int i;
};

int comparator(const OBJECT* visited_obj, const OBJECT* user_obj)
{
	int ret=0;
	MYOBJ *visited=(MYOBJ*) visited_obj;
	MYOBJ *user=(MYOBJ*) user_obj;

	if(visited && user)
	{
		if(user->i > visited->i)
			ret=1;
		else if(user->i < visited->i)
			ret=-1;
		else
			ret=0;
	}
	return ret;
}

int visitnode(OBJECT* obj, void *user_data)
{
	MYOBJ *myobj=(MYOBJ*)obj;

	if(fp && obj)
	{
		fprintf(fp, "%i\n", myobj->i);
	}

	return 1;
}

#define MAX 10000

int main(int argc, char** argv)
{
	int x[MAX];
	int r;
	int i;
	int tmp;
	MYOBJ* obj, del_obj;
#ifdef TEST_SORTED_LIST_DS
	//SORTED_LIST h;
#elif defined TEST_TREE_DS
	BINARY_TREE t;
#endif

	//srand((unsigned int)time(0));

	for(i=0; i<MAX; i++)
	{
		x[i]=i;
	}

	for(i=0; i<MAX; i++)
	{
		r=rand()%(MAX-i);
		obj=new MYOBJ;
		obj->i=x[r];
		tmp=x[MAX-i-1];
		x[r]=x[MAX-i-1];
		x[MAX-i-1]=tmp;
#ifdef TEST_SORTED_LIST_DS
		h.Insert(obj, comparator);
#elif defined TEST_TREE_DS
		t.Insert(comparator, obj);
#endif
	}

#ifdef TEST_SORTED_LIST_DS
	for(i=0; i<MAX; i++)
	{
		obj=(MYOBJ*)h.GetAt(i);
		printf("%i,", obj->i);
	}
#elif defined TEST_TREE_DS
	fp=fopen("out.txt", "wt");

	for(i=0; i<MAX/2; i++)
	{
		del_obj.i=i;
		t.Remove(comparator, &del_obj);
	}
	t.Traverse(visitnode, 0);
	fclose(fp);
#endif

	return 0;
}

#elif defined(TEST_AVL_TREE)


#define MAX_TO_INSERT 500

char random_names[][5]=
	{"znpv","xvdx","jemg","dtqx","uvzp","kzph","lnvq","ujfj","intc","lmyc","dfrc","frsw","sdcn","unrm","ijqc","sqjj",
	"xkun","qxhd","rwzr","ulbp","vchs","jvrs","fnht","gkwu","wdqx","lsux","tydd","lrwx","ezpy","vrbi","ozvq","njrs",
	"dqib","gckd","vblf","exbo","hzdc","vgil","hstg","zizo","fifl","ahwj","hfyk","wvvm","xglp","fllu","epwh","jyll",
	"ojcy","hhau","pdcc","bjem","aksr","zrrm","jmmz","hfkb","xrcj","hlxp","sbvw","xafl","hmed","xkjx","ptcc","vhhz",
	"dxeo","wqlg","qfht","otsv","dsxv","pegx","ykmc","nqoe","cffb","owik","yscz","ujcz","swwz","lqvp","jian","jpao",
	"aaxt","llbd","qqhj","zayq","kjge","hxxm","zcte","oxpo","cufn","anle","xpgc","yzeu","qplg","wmcq","guoy","qtpr",
	"htix","pqvy","hwaq","pibi","scst","ogxa","qmri","tghj","nmpw","xbcl","qhei","pvzm","ztxr","jykz","vuyw","sxmp",
	"tvjx","rhsg","txue","hjjf","bsbn","pbrd","fxud","hlwr","twta","lcxi","hmih","fyru","pyey","gstz","nbnz","qzps",
	"lgmu","puan","egga","hqkd","tfzn","oqvc","hgoj","royj","nhem","pjtq","hcad","shvp","lhhw","bnlg","mfrm","lwdi",
	"vvys","bglt","cwzb","xcnt","mlph","zfmd","sewy","ytvk","bzev","eaxt","vfan","cenh","ajmd","eiyw","oawi","ubax",
	"iuec","szqt","uxpz","ahfv","kkgz","tdom","uwuy","gvwi","eizn","dffx","fuzf","jzad","wgen","cwsm","ydot","euka",
	"ajcl","meoc","ajks","xoct","rzau","jhnv","llga","uepw","tfze","xlqc","rmix","gjtv","neot","wtot","bqzt","exhm",
	"zemc","ccng","slbz","kzuz","cvpu","zpiy","dqrx","ojku","oqyp","hdqz","yfjb","hyyh","qqaq","ybmz","teln","yrjn",
	"shhi","vnng","bklz","nqed","bvcy","vzkx","oxqn","utoo","fygb","yvpv","vcfp","ydjh","inuu","dctg","wmvs","lfsq",
	"osgu","gdfu","ykoc","drfb","nfeh","kusv","flnr","xwah","dscq","qprp","kivj","enyz","nxgb","whav","fnre","udbs",
	"sgza","edyo","vlig","fkds","cxhj","hxps","swof","sskk","xfmi","kjcx","vzhi","qmcd","tsjm","vpdl","hcip","xpbf",
	"mnob","kjqz","jakm","xreh","cotq","ooel","ycgz","pxbj","hwgy","qlcj","fhpy","qqci","htce","okua","pupe","qemb",
	"szql","tthc","gubr","fvza","xacq","ntky","vwcw","ucno","felb","cdya","qavo","psay","qxjd","tjwy","ekuf","ytab",
	"czff","jpqy","efxf","aild","zqwm","pbzd","xzwr","swwm","togl","vuus","pbio","kzdf","iypw","hfro","bely","nmkl",
	"vyrf","szom","phoq","hevp","mopt","tlia","mdvg","mfhg","gmzs","gijw","nvna","vvwo","zvrr","dodf","fyfb","dxyj",
	"omcv","zpth","bfjb","woqw","xfgd","jlrj","sxzl","ekng","junb","jljr","leak","uete","uwps","nmqn","ypfq","wzvl",
	"vmih","oonw","kofr","jvzh","iqph","tjwo","ppie","aclg","lpfb","rcwv","ipqg","rdgv","ylha","mhgw","ohvd","gvqj",
	"gnfx","swfb","fose","btpr","pgio","semk","ytyd","bdtl","eafu","cyro","lpqu","lphc","jpwk","vkdx","uznj","frvv",
	"uiix","eitv","ydwu","mtwq","dggw","dlqp","bcza","mhro","wffo","extx","bguh","vyzi","xpum","gcsz","xkqr","qtso",
	"iaui","upgq","feyy","hfoi","vpip","ucgh","nnhw","rtfz","gybe","wjps","umkf","bufk","nnnm","pdbk","xeqn","hhue",
	"uyva","ilfk","vqxn","rfeu","jvak","pbun","fzbp","nvio","gkmx","zpnt","gzyx","ygcn","ahhr","gutw","ltxr","fjqm",
	"nuxj","rwsa","bhrn","fpjd","shqh","tzxc","rzjr","avjt","jzpf","eqrg","kkrd","qmec","qskg","cdbs","vcpg","vrsb",
	"prpy","gbzj","znoo","ohhr","vmjx","llqa","fadj","cteo","ugmi","pijv","fckx","zixh","prew","cmnh","bhiy","bhaz",
	"mfss","xwvp","yver","fnrw","qsuf","djpm","rvuk","ctvn","pgqd","emuc","xztr","xaop","epyq","upnn","mrqi","bgjo",
	"zhux","sytj","qmft","ngpt","vnbw","tjzd","sexk","phxi","cvrm","lgfc","uuvb","sgnm","wjqz","xsqe","lhmr","mxzo",
	"lbzg","pmdj","tide","yqnl","iymh","usyd","doom","petw","sbfg","bsus","ncpx","cqtu","fwzy","ghcf","eckv","ayky",
	"aleg","hhpj","trkm","wjmy"};


int random_numbers[]=
	{25131521,2321323,94126,3191623,20212515,1025157,11132116,20959,813192,1112242,35172,
	5171822,183213,20131712,89162,181699,23102013,162373,17222517,2011115,212718,9211718,
	513719,6102220,2231623,11182023,192433,11172223,4251524,211718,14252116,1391718,31681,
	62103,211115,423114,72532,216811,718196,2582514,58511,7229,752410,22212112,2361115,
	5111120,415227,9241111,149224,77020,15322,19412,101817,25171712,9121225,75101,231729,
	7112315,1812122,230511,71243,2310923,151922,217725,323414,2216116,165719,14191821,3182321,
	154623,2410122,1316144,2551,1422810,2418225,209225,18222225,11162115,98013,915014,2319,
	111113,161679,2502416,10964,7232312,252194,14231514,220513,13114,231562,2425420,1615116,
	2212216,6201424,16191517,719823,15162124,722016,15818,1821819,146230,1612178,19679,13121522,
	231211,16748,15212512,25192317,9241025,21202422,18231215,1921923,177186,1923204,7995,118113,
	151173,523203,7112217,1922190,112238,71287,5241720,1524424,6181925,1311325,16251518,1161220,
	1520013,4660,716103,1952513,1416212,76149,1714249,137412,1591916,7203,1872115,117722,
	113116,1251712,112238,21212418,161119,222251,2321319,1211157,255123,1842224,24192110,125421,
	402319,215013,24137,9123,482422,140228,201023,82042,18251619,20231525,7521,1010625,
	1931412,20222024,621228,482513,35523,520255,92503,226413,2221812,2431419,420100,9211,
	124142,91018,2314219,1725020,971321,111160,2041522,195254,2311162,1712823,691921,1341419,
	22191419,1162519,423712,254122,22136,1811125,10252025,2211520,2515824,3161723,1491020,
	14162415,731625,24591,724247,1616016,2411225,1941113,2417913,18778,2113136,1101125,131643,
	121224,21251023,14231613,20191414,52461,24211521,212515,24397,8132020,32196,22122118,
	1151816,1418620,63520,2410142,31751,13547,10201821,5111317,232207,318216,16151715,108219,
	4132425,132361,227021,513174,203118,186250,432414,211186,510318,22379,7231518,1822145,
	18181010,235128,109223,212578,161223,1918912,2115311,72815,231515,1213141,1091625,901012,
	231747,2141916,1414411,242625,152319,722624,161129,571524,161628,71924,1410200,1520154,
	164121,18251611,191972,620117,521250,230216,13191024,2122222,2021314,54111,23240,1602114,
	1518024,162393,1992224,410205,241901,22555,9151624,45235,8113,25162212,151253,23252217,
	18222212,1914611,21202018,151814,102535,8241522,751714,141124,13121011,2124175,18251412,
	1571416,742115,12141519,191180,123216,12576,6122518,68922,1321130,21212214,25211717,31435,
	52451,323249,1412221,2515197,1591,22141622,23563,911179,18232511,410136,920131,911917,
	114010,204194,20221518,13121613,2415516,22252111,211287,14141322,1014517,921257,816157,
	1992214,151584,2116,111551,1722221,815166,173621,241170,127622,147213,621169,613523,
	182251,514184,1191517,156814,1841210,2419243,131911,40520,2241714,11151620,111572,9152210,
	2110323,2025139,5172121,208823,481921,2432220,12192216,36622,3111615,12250,1271714,225514,
	4231923,16207,2124258,23152012,621825,23101617,16191814,80208,2015616,542424,75148,
	2115815,20267,1313722,1719525,62414,2291518,2012105,120510,13131312,153110,2341613,77204,
	2024210,811510,21162313,175420,921010,1512013,525115,1321814,6101223,25151319,6252423,
	246213,7717,6201922,11192317,591612,1320239,1722180,171713,51593,187167,1925232,1725917,
	21919,925155,416176,1010173,161242,1618106,23118,212156,2117181,15171524,61259,25131414,
	147717,2112923,1111160,5039,219414,206128,158921,521023,258237,1517422,212137,17824,
	17025,1251818,23222115,2421417,5131722,1618205,391512,17212010,2192113,156163,412202,
	23251917,2301415,4152416,20151313,1217168,16914,2572023,1824199,1612519,1361519,2113122,
	199253,1842310,157238,2211712,11652,2020211,1861312,2291625,2318164,1171217,12232514,
	111256,151239,19834,24161311,824127,2018243,3141412,1541922,18156,1182018,1321523,2161920,
	5222524,6725,421021,241024,1146,77159,19171012,2291224};

///////////////////////////////////////////////////////////////////////////////////////////////
//typedef int(*AVL_TREE_FUNCTION_COMPARE)(void * pDataToInsert, void *pDataInTree); 
//typedef int(*AVL_TREE_FUNCTION_TRAVERSE)(void * pData); 

int compare(void *str1, void *str2)
{
	int  res = strcmp((const char*)str1, (const char*)str2);
	return res;
}

int compare2(void *num1, void *num2)
{
	int  res = (*(int*)num1 > *(int*)num2) ? 1:-1;
	return res;
}

void traverse(void *str)
{
	printf("%s, ", str);
}


int main(int argc, char* argv[])
{
	AVL_TREE tree;
	FILE* fp;
	int i;

	//int array[] = {80,30,15,10,5,20,40,100,90};
	//int array[] = {-80,-30,-15,-10,-5,-20,-40,-100,-90};
	//int array[] = {80,30,100,20,50,90,120,10,40,60,55};
	//int array[] = {-80,-30,-100,-20,-50,-90,-120,-10,-40,-60,-55};

	for(i=0; i<MAX_TO_INSERT; i++)
	//for(i=0; i<sizeof(array)/sizeof(int); i++)
	{
		tree.Insert((OBJECT*)random_names[i], (COMPARATOR)compare);
		//tree.Insert(&array[i], (AVL_TREE_FUNCTION_COMPARE)compare2);
		//char file[32];
		//sprintf(file, "out%03i.txt", i);
		//fp = fopen(file, "wb");
		//tree.Print(fp);
		//fclose(fp);
	}

	//tree.Delete(&random_names[31]);
	//tree.Delete(&random_names[48]);
	//tee.Delete(&random_names[27]);
	
	fp = fopen("out.txt", "wb");
	tree.Print(fp);
	fclose(fp);

	return 0;
}

#endif//TEST_AVL_TREE