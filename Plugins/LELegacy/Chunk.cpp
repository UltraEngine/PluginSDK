/*------------------------------------------------------------
Chunk class for reading Leadwerks MDL files.
This is just a temporary structure that gets used during loading.
------------------------------------------------------------*/

#include "DLLExports.h"

Chunk::Chunk() : id(0), size(0), countkids(0)
{}
	
Chunk::~Chunk()
{
	std::vector<Chunk*>::iterator it;
		
	for (it=kids.begin(); it!=kids.end(); it++)
	{
		delete (*it);
	}
	kids.clear();
}
	
void Chunk::Read(GMFSDK::MemReader& reader)
{
	Chunk* child = NULL;
	reader.Read(&id);
	reader.Read(&countkids);
	reader.Read(&size);
	pos = reader.Pos();
	reader.Seek(pos + size);
	for (int n=0; n<countkids; n++)
	{
		child = new Chunk;
		child->Read(reader);
		kids.push_back(child);
	}
}