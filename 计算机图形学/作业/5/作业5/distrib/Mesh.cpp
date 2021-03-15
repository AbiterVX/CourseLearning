#include "Mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>

#define SMOOTH (v.size()>120)

void intersectCall(int idx, void ** arg)
{
	Mesh * m = (Mesh*)(arg[0]);
	bool result = m->intersectTrig(idx);
	arg[1] = (void*)(((bool)arg[1])|result);
}
bool Mesh::intersect( const Ray& r , Hit& h , float tmin )
{
/*	bool result = false;
	for( unsigned int i = 0 ; i < t.size() ; i++){
		Triangle triangle(v[t[i][0]],
			v[t[i][1]],v[t[i][2]],material);
		for(int jj=0;jj<3;jj++){
			triangle.normals[jj] = n[t[i][jj]];

		}
		if(texCoord.size()>0){
			for(int jj=0;jj<3;jj++){
				triangle.texCoords[jj] = texCoord[t[i].texID[jj]];
			}
			triangle.hasTex=true;
		}
		result |= triangle.intersect( r , h , tmin);
	}
	return result;
	*/
	ray = &r;
	hit = &h;
	tm = tmin;
	void * arg[2];
	arg[0] = this;
	arg[1] = 0;
	octree.arg = arg;
	octree.termFunc = intersectCall;
	octree.intersect(r);
	return arg[1];
}
bool Mesh ::intersectTrig(int idx){
	bool result = false;
	Triangle triangle(v[t[idx][0]],
		v[t[idx][1]],v[t[idx][2]],material);
	
  //some shitty hack
//change at will
if(SMOOTH){

    for(int jj=0;jj<3;jj++){
		  triangle.normals[jj] = n[t[idx][jj]];
	  }
	}else{
    
    for(int jj=0;jj<3;jj++){
		  triangle.normals[jj] = n[idx];
	  }
}
  if(texCoord.size()>0){
		for(int jj=0;jj<3;jj++){
			triangle.texCoords[jj] = texCoord[t[idx].texID[jj]];
		}
		triangle.hasTex=true;
	}
	result = triangle.intersect( *ray , *hit , tm);
	return result;
}
Mesh::Mesh(const char * filename,Material * material):Object3D(material)
{
	std::ifstream f ;
	f.open(filename);
	if(!f.is_open()) {
		std::cout<<"Cannot open "<<filename<<"\n";
		return;
	}
	std::string line;
	std::string vTok("v");
	std::string fTok("f");
	std::string texTok("vt");
	char bslash='/',space=' ';
	std::string tok;
	while(1) {
		std::getline(f,line);
		if(f.eof()) {
			break;
		}
		if(line.size()<3) {
			continue;
		}
		if(line.at(0)=='#') {
			continue;
		}
		std::stringstream ss(line);
		ss>>tok;
		if(tok==vTok) {
			Vector3f vec;
			ss>>vec[0]>>vec[1]>>vec[2];
			v.push_back(vec);
		} else if(tok==fTok) {
			if(line.find(bslash)!=std::string::npos) {
				std::replace(line.begin(),line.end(),bslash,space);
				std::stringstream facess(line);
				Trig trig;
				facess>>tok;
				for(int ii=0; ii<3; ii++) {
					facess>>trig[ii]>>trig.texID[ii];
					trig[ii]--;
					trig.texID[ii]--;
				}
				t.push_back(trig);
			} else {
				Trig trig;
				for(int ii=0; ii<3; ii++) {
					ss>>trig[ii];
					trig[ii]--;
					trig.texID[ii]=0;
				}
				t.push_back(trig);
			}
		} else if(tok==texTok) {
			Vector2f texcoord;
			ss>>texcoord[0];
			ss>>texcoord[1];
			texCoord.push_back(texcoord);
		}
	}
	f.close();
	compute_norm();
	octree.build(*this);
}

void Mesh::compute_norm()
{
if (SMOOTH){
	n.resize(v.size());
	for(unsigned int ii=0; ii<t.size(); ii++) {
		Vector3f a = v[t[ii][1]] - v[t[ii][0]];
		Vector3f b = v[t[ii][2]] - v[t[ii][0]];
		b=Vector3f::cross(a,b);
		for(int jj=0; jj<3; jj++) {
			n[t[ii][jj]]+=b;
		}
	}
	for(unsigned int ii=0; ii<v.size(); ii++) {
		n[ii] = n[ii]/ n[ii].abs();
	}
}else{
	n.resize(t.size());
	for(unsigned int ii=0; ii<t.size(); ii++) {
		Vector3f a = v[t[ii][1]] - v[t[ii][0]];
		Vector3f b = v[t[ii][2]] - v[t[ii][0]];
		b=Vector3f::cross(a,b);
	  n[ii]=b.normalized();
	}

}
}
