#include <list>
#include "scene-graph.h"

using namespace std;

PlaceNode* buildHouse(float x, float y, float s, 
					  colorType bc, bool bf, 
					  colorType rc, bool rf,
					  colorType vc, bool vf)
{
   Rectangle* base = new Rectangle(s,s,bc,bf);
   PlaceNode* basePlaced = new PlaceNode(base,Location(0.0,0.0));
   Triangle* roof = new Triangle(s,s/2.0,rc,rf);
   PlaceNode* roofPlaced = new PlaceNode(roof,Location(0.0,s)); 
   Circle* vent = new Circle(s/8.0,vc,vf);
   PlaceNode* ventPlaced = new PlaceNode(vent,Location(s/2.0,s*19.0/16.0)); 

   list<PlaceNode*> nodeList;
   nodeList.push_back(basePlaced);
   nodeList.push_back(roofPlaced);
   nodeList.push_back(ventPlaced);

   return new PlaceNode(nodeList,Location(x,y));
}


PlaceNode* buildFancyHouse(float x, float y, float s,
						   colorType hbc, bool hbf, 
						   colorType hrc, bool hrf,
						   colorType hvc, bool hvf,
						   colorType gbc, bool gbf, 
						   colorType grc, bool grf,
						   colorType gvc, bool gvf)
{
   PlaceNode* house = buildHouse(0.0,0.0,s,hbc,hbf,hrc,hrf,hvc,hvf);
   PlaceNode* garage = buildHouse(s,0.0,s/2.0,gbc,gbf,grc,grf,gvc,gvf);
   list<PlaceNode*> nodeList;
   nodeList.push_back(house);
   nodeList.push_back(garage);
   return new PlaceNode(nodeList,Location(x,y));
}