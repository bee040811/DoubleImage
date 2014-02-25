
class double_image{
public:
	
	// saveSelectedMesh
	void saveSelectedMeshFile(string filename,vector<bool> FaceSelect);

	// load selected mesh file
	void loadSelectedMeshFile(string filename,vector<bool> &FaceSelect);

	// clear all Selected mesh quad 
	void SelectedMeshReset(vector<bool> &FaceSelect);
};

