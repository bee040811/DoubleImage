/*
	new main ,to new double image file

*/

#include "main.h"

void double_image::saveSelectedMeshFile(string filename,vector<bool>  FaceSelect) {
	// save selected mesh 
	// need to save into XML file 
	// by map iterator method to save combine way & node 
	// write xml file 
	map<int, map<int,unsigned long long int>>::iterator it;
	map<int,unsigned long long int>::iterator it_inner;

	System::Xml::XmlDocument^ file = gcnew XmlDocument();
	file->LoadXml("<mesh></mesh>");

	for(int i = 0; i < FaceSelect.size() ; i++) {
		if(FaceSelect[i] == true) {
			XmlElement^ elem = file->CreateElement( "nodes" );
			elem->SetAttribute("id",i.ToString());
			file->DocumentElement->AppendChild( elem );
		}
	}

	System::String^ filenameRef = gcnew  System::String(filename.c_str());
	file->Save(filenameRef);
}

void double_image::loadSelectedMeshFile(string filename,vector<bool> &FaceSelect) {
	// save selected mesh 
	// need to save into XML file
	map<int, map<int,unsigned long long int>>::iterator it;
	map<int,unsigned long long int>::iterator it_inner;
	System::Xml::XmlDocument^ file = gcnew XmlDocument();
	file->Load( gcnew System::String( filename.c_str()) );
	XmlNode^ root = file->DocumentElement;
	XmlNodeList^ nodeList = root->SelectNodes("nodes");
	System::Collections::IEnumerator^ nodeListEnum = nodeList->GetEnumerator();
	while(nodeListEnum->MoveNext()) {
		XmlNode^ perNode;
		perNode = safe_cast<XmlNode^>(nodeListEnum->Current);
		unsigned long long int id = Convert::ToInt32(perNode->Attributes->GetNamedItem("id")->Value);
		FaceSelect[id] = true;
	}
}

void double_image::SelectedMeshReset(vector<bool> &FaceSelect) {
	for(int i =0 ; i < FaceSelect.size() ; i++) {
		FaceSelect[i] = false;
	}
	
}