#include "main.h"

bool manager_image::SaveImage(string filename)
{
	GLubyte *bits = NULL , *flipbits = NULL;
	int rec_width = 0 , rec_height = 0;
	vector2 store_resolution;
	store_resolution.x = vertexList[rbIdx].x;
	store_resolution.y = vertexList[rbIdx].y;
	// 	int img_width = (int)(currRes.x + 0.5f) * 2 + 20;
	// 	int img_height = (int)(currRes.y + 0.5f) * 2 + 20;
	int img_width = (int)(store_resolution.x) ;
	int img_height = (int)(store_resolution.y );



	if (bits != NULL)		delete [] bits;
	if (flipbits != NULL)	delete [] flipbits;
	if (m_output != NULL)	delete m_output;

	rec_width = img_width;
	rec_height = img_height;

	bits = new GLubyte[rec_width * rec_height * 4];
	flipbits = new GLubyte[rec_width * rec_height * 4];

	m_output = new IplImage;
	memset((void *)m_output , 0 , sizeof(IplImage));

	m_output->nSize = sizeof(IplImage);
	m_output->align = 4;
	m_output->depth = 8;
	m_output->nChannels = 4;
	m_output->width = rec_width;
	m_output->height = rec_height;
	m_output->colorModel[0] = 'R';		m_output->colorModel[1] = 'G';		m_output->colorModel[2] = 'B';
	m_output->channelSeq[0] = 'B';		m_output->channelSeq[1] = 'G';		m_output->channelSeq[2] = 'R';
	m_output->imageSize = rec_width * rec_height * m_output->nChannels;
	m_output->widthStep = rec_width * m_output->nChannels;
	m_output->imageData = (char *)flipbits;
	m_output->imageDataOrigin = (char *)bits;

	// result
	//glReadPixels((int)ImagePos.x - 5.0f , (int)(panelSize.y - ImagePos.y - currRes.y - 10.0f) , rec_width , rec_height , GL_BGRA_EXT , GL_UNSIGNED_BYTE , bits);

	// everything
	//glReadPixels((int)ImagePos.x - 5.0f , (int)(panelSize.y - ImagePos.y - 2*currRes.y - 10.0f) , rec_width , rec_height , GL_BGRA_EXT , GL_UNSIGNED_BYTE , bits);


	glReadPixels((int)ImagePos.x , (int)(panelSize.y - ImagePos.y - img_height )  , rec_width , rec_height , GL_BGRA_EXT , GL_UNSIGNED_BYTE , bits);

	for (int i = 0 ; i < rec_height ; i++)
		memcpy(	flipbits + i * m_output->widthStep , bits + (rec_height - i - 1) * m_output->widthStep , m_output->widthStep);

	cvSaveImage(filename.c_str() , m_output);

	return true;
}

void manager_image::OpenGLinitial()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Set Texture Max Filter
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Set Texture Min Filter

	GLfloat  ambientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat  diffuseLight[] = { 0.8f, 0.5f, 0.0f, 1.0f };
	GLfloat  specular[] = { 0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat  Lposition[] = {0.0f, 10.0f, 0.0f , 1.0f};

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glLightfv(GL_LIGHT0,GL_POSITION,Lposition);
	glEnable(GL_LIGHT0);

	glClearColor(1.0f , 1.0f , 1.0f , 1.0f);
}

void manager_image::PanelResize(int width, int height)
{
	if (height==0)										// Prevent A Divide By Zero By
		height=1;										// Making Height Equal One

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	gluOrtho2D(0, width, height, 0);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();
}

void manager_image::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();

	glPushMatrix();
	glTranslatef(ImagePos.x , ImagePos.y + Zoom * currRes.y  , 0.0f);
	glScalef(Zoom , Zoom , 1.0f);

	if(Original_Image)  
	{
		RenderContent();
		RenderLinearScale();
	}			
	if(Render_Saliency)               RenderSaliency();
	if(Render_Saliency2)              RenderSaliency2();
	if(Render_mask && Mask_Image)     RenderMask();
	if(Render_mesh)                   RenderMesh();
	if(Render_Faceselecct)            RenderFaceSelect();     

	if(Render_segmentation) 
	{
		glPushMatrix();
		//		glTranslatef( Image_width + 5 , 0.0f , 0.0f);
		RenderSegmentation();
		glPopMatrix();
	}
	if(Render_patchsaliency)          
	{
		glPushMatrix();
		//		glTranslatef( Image_width + 5 , Image_height + 5.0 , 0.0f);
		if(Render_mesh) RenderMesh();
		RenderPatchSaliency();

		glPopMatrix();
	}
	if(Render_patchCenter) {
		RenderPatchCenter();	
	}
	// 	for(int i = 0 ; i < constraintLines.size() ; ++i)
	// 	{
	// 		glColor3f(0.0,1.0,1.0);
	// 		glLineWidth(3.0);
	// 		glBegin(GL_LINES);
	// 		glVertex2f(constraintLines[i][0].x,constraintLines[i][0].y);
	// 		glVertex2f(constraintLines[i][1].x,constraintLines[i][1].y);
	// 		glEnd();
	// 	}

	// 	for(int i = 0 ; i < detectedge.size() ; ++i)
	// 	{
	// 		for(int j = 0 ; j < detectedge[i].size() ; ++j)
	// 		{
	// 			int index_1 = detectedge[i][j].x;
	// 			int index_2 = detectedge[i][j].y;
	// 			glColor3f(0.0,1.0,1.0);
	// 			glLineWidth(3.0);
	// 			glBegin(GL_LINES);
	// 			glVertex2f(vertexList[index_1].x,vertexList[index_1].y);
	// 			glVertex2f(vertexList[index_2].x,vertexList[index_2].y);
	// 			glEnd();
	// 		}
	// 
	// 	}



	if(Original_Image)
	{
		glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
		glColor3f(1.0,1.0,1.0);
		glBegin(GL_QUADS);
		/*
		glVertex2f(vertexList[rbIdx].x,0);
		glVertex2f(vertexList[rbIdx].x+Image_width,0);
		glVertex2f(vertexList[rbIdx].x+Image_width,vertexList[rbIdx].y);
		glVertex2f(vertexList[rbIdx].x,vertexList[rbIdx].y);

		glVertex2f(0.0,0.0);
		glVertex2f(-Image_width,0);
		glVertex2f(-Image_width,vertexList[rbIdx].y);
		glVertex2f(0.0,vertexList[rbIdx].y);
		*/
		// 		glVertex2f(0.0,0.0);
		// 		glVertex2f(0,-Image_height);
		// 		glVertex2f(vertexList[rbIdx].x,-Image_height);
		// 		glVertex2f(vertexList[rbIdx].x,0);
		// 
		// 		glVertex2f(0.0,vertexList[rbIdx].y);
		// 		glVertex2f(vertexList[rbIdx].x,vertexList[rbIdx].y);
		// 		glVertex2f(vertexList[rbIdx].x,vertexList[rbIdx].y+Image_height);
		// 		glVertex2f(0.0,vertexList[rbIdx].y+Image_height);
		glEnd();

	}

	//if(Render_Faceselecct) RenderFaceSelect();


	glPopMatrix();
}

void manager_image::RenderContent()
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Original_Image->width, Original_Image->height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, Original_Image->imageData);

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f , 1.0f , 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);

	// 	glBegin(GL_QUADS);
	// 
	// 	glTexCoord2f(0.0,0.0);
	// 	glVertex2f(0.0,0.0);
	// 
	// 	glTexCoord2f(1.0,0.0);
	// 	glVertex2f(Original_Image->width,0.0);
	// 
	// 	glTexCoord2f(1.0,1.0);
	// 	glVertex2f(Original_Image->width,Original_Image->height);
	// 
	// 	glTexCoord2f(0.0,1.0);
	// 	glVertex2f(0.0,Original_Image->height);
	// 
	// 	glEnd();

	if(triangle)
	{
		glBegin(GL_TRIANGLES);
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[3] = {faceList[i].x,faceList[i].y,faceList[i].z};

			glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
			glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);

			glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
			glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);

			glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
			glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);
		}
		glEnd();
	}
	else
	{
		glBegin(GL_QUADS);
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};

			glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
			glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);

			glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
			glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);

			glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
			glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);

			glTexCoord2f(referenceList[vList[3]].x / Image_width,referenceList[vList[3]].y / Image_height);
			glVertex2f(vertexList[vList[3]].x,vertexList[vList[3]].y);
		}
		glEnd();
	}
	//	printf("coordinate : (%f, %f)\n",vertexList[rbIdx].x,vertexList[rbIdx].y);

	glDisable(GL_TEXTURE_2D);


}


void manager_image::RenderMesh()
{
	glPolygonMode(GL_FRONT_AND_BACK , GL_LINE);
	glLineWidth(2.0);
	glColor3f(0.6f , 0.0f , 0.8f);

	if(triangle)
	{
		glBegin(GL_TRIANGLES);
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[3] = {faceList[i].x,faceList[i].y,faceList[i].z};

			//glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
			glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);

			//glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
			glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);

			//glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
			glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);
		}
		glEnd();
	}
	else
	{
		glBegin(GL_QUADS);
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};

			//glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
			glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);

			//glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
			glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);

			//glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
			glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);

			//glTexCoord2f(referenceList[vList[3]].x / Image_width,referenceList[vList[3]].y / Image_height);
			glVertex2f(vertexList[vList[3]].x,vertexList[vList[3]].y);
		}
		glEnd();

		// 		for (int i = 0; i <patch_center_edge.size(); i++)
		// 		{
		// 			glColor3f(0.0,1.0,1.0);
		// 			glLineWidth(3.0);
		// 			glBegin(GL_LINES);
		// 			glVertex2f(referenceList[patch_center_edge[i].x].x,referenceList[patch_center_edge[i].x].y);
		// 			glVertex2f(referenceList[patch_center_edge[i].y].x,referenceList[patch_center_edge[i].y].y);
		// 			glEnd();
		// 		}

	}

	if(Mask_Image && triangle)
	{
		glColor3f(0.0,1.0,0.0);
		glPointSize(5.0);
		glBegin(GL_POINTS);
		glVertex2f(referenceList[center_edge.x].x,referenceList[center_edge.x].y);
		glVertex2f(referenceList[center_edge.y].x,referenceList[center_edge.y].y);
		glEnd();
	}

	for(int i = 0 ; i < detectedge.size() ; ++i)
	{
		for(int j = 0 ; j < detectedge[i].size() ; ++j)
		{
			int index_1 = detectedge[i][j].x;
			int index_2 = detectedge[i][j].y;
			glColor3f(0.0,1.0,1.0);
			glLineWidth(3.0);
			glBegin(GL_LINES);
			glVertex2f(vertexList[index_1].x,vertexList[index_1].y);
			glVertex2f(vertexList[index_2].x,vertexList[index_2].y);
			glEnd();
		}

	}
}

void manager_image::RenderSaliency()
{

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Original_Image->width, Original_Image->height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, Saliency_Map->imageData);

	//glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f , 1.0f , 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);

	// 	glBegin(GL_QUADS);
	// 
	// 	glTexCoord2f(0.0,0.0);
	// 	glVertex2f(0.0,0.0);
	// 
	// 	glTexCoord2f(1.0,0.0);
	// 	glVertex2f(Original_Image->width,0.0);
	// 
	// 	glTexCoord2f(1.0,1.0);
	// 	glVertex2f(Original_Image->width,Original_Image->height);
	// 
	// 	glTexCoord2f(0.0,1.0);
	// 	glVertex2f(0.0,Original_Image->height);
	// 
	// 	glEnd();
	if(triangle)
	{
		glBegin(GL_TRIANGLES);
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[3] = {faceList[i].x,faceList[i].y,faceList[i].z};

			vector3 color = GrayToColor(faceflexibleList[i]);
			glColor3f(color.x,color.y,color.z);
			//glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
			glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);

			//glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
			glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);

			//glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
			glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);
		}
		glEnd();
	}
	else
	{
		for(int i = 0 ; i < Image_width ; ++i)
		{
			for(int j= 0 ; j < Image_height ; ++j)
			{
				//unsigned float s = (double)Saliency_Map->imageData[j*Saliency_Map->widthStep + 3*i];
				uchar s = Saliency_Map->imageData[j*Saliency_Map->widthStep + 3*i /*+ 2*/];
				vector3 color = GrayToColor((double)s / 255);
				//  				uchar* ptr = (uchar*)(Saliency_Map->imageData + j * Saliency_Map->widthStep + 3*i);
				//  				vector3 color = GrayToColor((double)ptr[0]/255);
				glColor3f(color.x,color.y,color.z);
				//glPointSize(5.0);
				//glBegin(GL_POINTS)
				glBegin(GL_QUADS);
				glVertex2f(i,j);
				glVertex2f(i+1,j);
				glVertex2f(i+1,j+1);
				glVertex2f(i,j+1);
				glEnd();
			}
		}

		// 		glBegin(GL_QUADS);
		// 		for(int i = 0 ; i < faceList.size() ; ++i)
		// 		{
		// 			int vList[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};
		// 
		// 			vector3 color = GrayToColor(faceflexibleList[i]);
		// 			glColor3f(color.x,color.y,color.z);
		// 			//glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
		// 			glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);
		// 
		// 			//glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
		// 			glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);
		// 
		// 			//glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
		// 			glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);
		// 
		// 			//glTexCoord2f(referenceList[vList[3]].x / Image_width,referenceList[vList[3]].y / Image_height);
		// 			glVertex2f(vertexList[vList[3]].x,vertexList[vList[3]].y);
		// 		}
		// 		glEnd();
	}
	//glDisable(GL_TEXTURE_2D);
}

void manager_image::RenderSaliency2()
{

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Original_Image->width, Original_Image->height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, Saliency_Map->imageData);

	//glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f , 1.0f , 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);

	// 	glBegin(GL_QUADS);
	// 
	// 	glTexCoord2f(0.0,0.0);
	// 	glVertex2f(0.0,0.0);
	// 
	// 	glTexCoord2f(1.0,0.0);
	// 	glVertex2f(Original_Image->width,0.0);
	// 
	// 	glTexCoord2f(1.0,1.0);
	// 	glVertex2f(Original_Image->width,Original_Image->height);
	// 
	// 	glTexCoord2f(0.0,1.0);
	// 	glVertex2f(0.0,Original_Image->height);
	// 
	// 	glEnd();
	if(triangle)
	{
		glBegin(GL_TRIANGLES);
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[3] = {faceList[i].x,faceList[i].y,faceList[i].z};

			vector3 color = GrayToColor(saliency2[i]);
			glColor3f(color.x,color.y,color.z);
			//glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
			glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);

			//glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
			glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);

			//glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
			glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);
		}
		glEnd();
	}
	else
	{
		glBegin(GL_QUADS);
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};

			vector3 color = GrayToColor(saliency2[i]);
			glColor3f(color.x,color.y,color.z);
			//glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
			glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);

			//glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
			glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);

			//glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
			glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);

			//glTexCoord2f(referenceList[vList[3]].x / Image_width,referenceList[vList[3]].y / Image_height);
			glVertex2f(vertexList[vList[3]].x,vertexList[vList[3]].y);
		}
		glEnd();
	}
	//glDisable(GL_TEXTURE_2D);
}

void manager_image::RenderPatchSaliency()
{

	if(triangle)
	{
		glColor3f(0.0,0.0,0.0);
		for(int i = 0 ; i < patch_num ;++i)
		{
			vector3 color = GrayToColor(patch[i].saliency);
			for(int j = 0 ; j < patch[i].triangle.size() ; ++j)
			{
				int index = patch[i].triangle[j];
				glBegin(GL_TRIANGLES);
				int vList[3] = {faceList[index].x,faceList[index].y,faceList[index].z};

				glColor3f(color.x,color.y,color.z);

				glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);
				glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);
				glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);

				glEnd();
			}
		}
	} else {

		glColor3f(0.0,0.0,0.0);
		for(int i = 0 ; i < patch_num ;++i) {
			vector3 color = GrayToColor(patch[i].saliency);
			for(int j = 0 ; j < patch[i].quad.size() ; ++j) {
				int index = patch[i].quad[j];
				glBegin(GL_QUADS);
				int vList[4] = {faceList[index].x,faceList[index].y,faceList[index].z,faceList[index].w};

				glColor3f(color.x,color.y,color.z);

				glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);
				glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);
				glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);
				glVertex2f(vertexList[vList[3]].x,vertexList[vList[3]].y);

				glEnd();
			}
		}

	}

}
void manager_image::RenderPatchCenter() {	
	for(int i = 0 ; i < patch_center_edge.size() ; i++) {
		glColor3f(255,0,0);
		glBegin(GL_LINES);
		glVertex2f(vertexList[patch_center_edge[i].x].x,vertexList[patch_center_edge[i].x].y);
		glVertex2f(vertexList[patch_center_edge[i].y].x,vertexList[patch_center_edge[i].y].y);

		glEnd();
	}
}

void manager_image::RenderSegmentation()
{
	glColor3f(1.0,1.0,1.0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Image_width, Image_height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, Graph_Seg->imageData);

	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	glBegin(GL_QUADS);

	float x_interval = 20.0 / Image_width;
	float y_interval = 20.0 / Image_height;

	for(float i = 0 ; i < 1 ; i +=x_interval)
	{
		for(float j = 0 ; j < 1 ; j +=y_interval)
		{
			float right = i + x_interval;
			float down  = j + y_interval;
			if(right > 1) right = 1;
			if(down  > 1) down  = 1;

			glTexCoord2f(i, j);
			glVertex2f(i*Image_width, j*Image_height);


			glTexCoord2f(right, j);
			glVertex2f((right)*Image_width, j*Image_height);


			glTexCoord2f(right, down);
			glVertex2f((right)*Image_width,(down)*Image_height);


			glTexCoord2f(i, down);
			glVertex2f(i*Image_width, (down)*Image_height);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

}

void manager_image::RenderFaceSelect()
{
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR , GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.5f , 0.2f , 0.3f , 0.5f);


	for (unsigned i = 0 ; i < FaceSelect.size() ; i++)
	{
		//if (!FaceSelect[i])	continue;

		if(FaceSelect[i])
		{
			int Idx[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};

			if(triangle)
			{
				glBegin(GL_TRIANGLES);
				glVertex2f(vertexList[Idx[0]].x,vertexList[Idx[0]].y);
				glVertex2f(vertexList[Idx[1]].x,vertexList[Idx[1]].y);
				glVertex2f(vertexList[Idx[2]].x,vertexList[Idx[2]].y);
				glEnd();
			}
			else
			{
				glBegin(GL_QUADS);
				glVertex2f(vertexList[Idx[0]].x,vertexList[Idx[0]].y);
				glVertex2f(vertexList[Idx[1]].x,vertexList[Idx[1]].y);
				glVertex2f(vertexList[Idx[2]].x,vertexList[Idx[2]].y);
				glVertex2f(vertexList[Idx[3]].x,vertexList[Idx[3]].y);
				glEnd();
			}

		}



	}
	glDisable(GL_BLEND);
}

void manager_image::RenderMask()
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Mask_Image->width, Mask_Image->height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, Mask_Image->imageData);

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f , 1.0f , 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);

	if(triangle)
	{
		glBegin(GL_TRIANGLES);
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[3] = {faceList[i].x,faceList[i].y,faceList[i].z};

			glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
			glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);

			glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
			glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);

			glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
			glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);
		}
		glEnd();
	}
	else
	{
		glBegin(GL_QUADS);
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};

			glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
			glVertex2f(vertexList[vList[0]].x,vertexList[vList[0]].y);

			glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
			glVertex2f(vertexList[vList[1]].x,vertexList[vList[1]].y);

			glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
			glVertex2f(vertexList[vList[2]].x,vertexList[vList[2]].y);

			glTexCoord2f(referenceList[vList[3]].x / Image_width,referenceList[vList[3]].y / Image_height);
			glVertex2f(vertexList[vList[3]].x,vertexList[vList[3]].y);
		}
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}

void manager_image::RenderLinearScale()
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Original_Image->width, Original_Image->height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, Original_Image->imageData);

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f , 1.0f , 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);

	glPushMatrix();

	float linear = 2;

	if(vertexList[rbIdx].x == referenceList[rbIdx].x) 
	{
		linear = 1;
		glTranslatef(0.0,5+vertexList[rbIdx].y,0.0);
	}
	else
	{
		glTranslatef(0.0,5+vertexList[rbIdx].y,0.0);
	}

	if(triangle)
	{

		float x_ratio = vertexList[rbIdx].x / Image_width;
		float y_ratio = vertexList[rbIdx].y / Image_height;
		glBegin(GL_TRIANGLES);
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[3] = {faceList[i].x,faceList[i].y,faceList[i].z};

			glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
			glVertex2f(referenceList[vList[0]].x*x_ratio,referenceList[vList[0]].y*y_ratio);

			glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
			glVertex2f(referenceList[vList[1]].x*x_ratio,referenceList[vList[1]].y*y_ratio);

			glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
			glVertex2f(referenceList[vList[2]].x*x_ratio,referenceList[vList[2]].y*y_ratio);
		}
		glEnd();
	}

	// 	if(triangle)
	// 	{
	// 		glBegin(GL_TRIANGLES);
	// 		for(int i = 0 ; i < faceList.size() ; ++i)
	// 		{
	// 			int vList[3] = {faceList[i].x,faceList[i].y,faceList[i].z};
	// 
	// 			glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
	// 			glVertex2f(referenceList[vList[0]].x,referenceList[vList[0]].y/linear);
	// 
	// 			glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
	// 			glVertex2f(referenceList[vList[1]].x,referenceList[vList[1]].y/linear);
	// 
	// 			glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
	// 			glVertex2f(referenceList[vList[2]].x,referenceList[vList[2]].y/linear);
	// 		}
	// 		glEnd();
	// 	}

	else
	{
		float x_ratio = vertexList[rbIdx].x / Image_width;
		float y_ratio = vertexList[rbIdx].y / Image_height;
		glBegin(GL_QUADS);
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};

			glTexCoord2f(referenceList[vList[0]].x / Image_width,referenceList[vList[0]].y / Image_height);
			glVertex2f(referenceList[vList[0]].x*x_ratio,referenceList[vList[0]].y*y_ratio);

			glTexCoord2f(referenceList[vList[1]].x / Image_width,referenceList[vList[1]].y / Image_height);
			glVertex2f(referenceList[vList[1]].x*x_ratio,referenceList[vList[1]].y*y_ratio);

			glTexCoord2f(referenceList[vList[2]].x / Image_width,referenceList[vList[2]].y / Image_height);
			glVertex2f(referenceList[vList[2]].x*x_ratio,referenceList[vList[2]].y*y_ratio);

			glTexCoord2f(referenceList[vList[3]].x / Image_width,referenceList[vList[3]].y / Image_height);
			glVertex2f(referenceList[vList[3]].x*x_ratio,referenceList[vList[3]].y*y_ratio);
		}
		glEnd();
	}
	//	printf("coordinate : (%f, %f)\n",vertexList[rbIdx].x,vertexList[rbIdx].y);

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void manager_image::LoadCluster(std::string filename)
{
	ifstream fp1(filename.c_str()) ;

	int cluster_num;
	int num;

	fp1>>cluster_num;

	while(!fp1.eof())
	{
		fp1>>num;
		cluster_index.push_back(num);
	}

	fp1.close();

	patch_num = cluster_num;

	CalculateEdge();

	double time__1 = clock();

	SimplyGraph();
	printf("merge time:%f\n",(clock()-time__1)/CLK_TCK);
	int index = 0;
	for(int i = 0 ; i < patch_num ; i++)
	{
		if(patch[i].number != 1)
		{
			patch[i].path_id = index;
			index++;
		}
		else
		{
			patch[i].path_id = -1;
		}
	}

	valid_patch_num = index;

	// 	 SortEdge();

	for (int y = 0; y < Image_height; y++) 
	{
		for (int x = 0; x < Image_width; x++) 
		{
			int index = cluster_index[y*Image_width + x];
			Graph_Seg->imageData[y*Graph_Seg->widthStep + 3*x]    = patch[index].color_B; 
			Graph_Seg->imageData[y*Graph_Seg->widthStep + 3*x +1] = patch[index].color_G; 
			Graph_Seg->imageData[y*Graph_Seg->widthStep + 3*x +2] = patch[index].color_R; 
		}
	}



	/*
	char t;
	fscanf(fp1,"%c",&t);

	path temp;
	patch.push_back(temp);

	patch[0].color_B = rand()%256;
	patch[0].color_G = rand()%256;
	patch[0].color_R = rand()%256;
	int now_patch = 0;
	int x;
	int y;
	while(!feof(fp1))
	{
	while(t!= '<') 
	{
	fscanf(fp1,"%c",&t);
	if(t == 'c') 
	{
	patch.push_back(temp);
	now_patch++;
	patch[now_patch].color_B = rand()%256;
	patch[now_patch].color_G = rand()%256;
	patch[now_patch].color_R = rand()%256;
	}
	if(feof(fp1)) break;
	}

	fscanf(fp1,"%d",&x);
	fscanf(fp1,"%d",&y);
	patch[now_patch].px.push_back(x);
	patch[now_patch].py.push_back(y);
	fscanf(fp1,"%c",&t);
	if(feof(fp1)) break;

	}
	*/
}

void manager_image::LoadImportantMap(string filename)
{
	IplImage *temp;
	Saliency_Map = cvCreateImage(cvSize(Image_width,Image_height),IPL_DEPTH_8U,3);
	temp = cvLoadImage(filename.c_str(),CV_LOAD_IMAGE_COLOR);

	for(int j = 0 ; j < temp->height ; j++)
	{
		for(int i = 0 ; i < temp->width ; i++)
		{
			temp->imageData[j*temp->widthStep + 3*i + 2] = temp->imageData[j*temp->widthStep + 3*i + 2];
			temp->imageData[j*temp->widthStep + 3*i]     = temp->imageData[j*temp->widthStep + 3*i];
			temp->imageData[j*temp->widthStep + 3*i + 1] = temp->imageData[j*temp->widthStep + 3*i + 1];
			// 			temp->imageData[j*temp->widthStep + 3*i + 2] = 255 - temp->imageData[j*temp->widthStep + 3*i + 1];
			// 			temp->imageData[j*temp->widthStep + 3*i]     = 255 - temp->imageData[j*temp->widthStep + 3*i + 1];
			// 			temp->imageData[j*temp->widthStep + 3*i + 1] = 255 - temp->imageData[j*temp->widthStep + 3*i + 1];
		}
	}


	cvResize(temp,Saliency_Map);

	//cvResize(temp, Original_Image);
}

void manager_image::CalculateEdge()
{
	path temp;

	vector<int> sum_r;
	vector<int> sum_g;
	vector<int> sum_b;
	vector<int> num_color;

	global_boundary.clear();

	for(int i = 0 ; i < patch_num ; i++)  
	{
		patch.push_back(temp);
		patch[i].color_B = rand()%256;
		patch[i].color_G = rand()%256;
		patch[i].color_R = rand()%256;
		patch[i].path_id = i;

		sum_b.push_back(0);
		sum_g.push_back(0);
		sum_r.push_back(0);
		num_color.push_back(1);
	}

	for(int y = 0 ; y < Image_height ; y++)
	{
		int temp_index;
		for(int x = 0 ; x < Image_width ; x++)
		{
			uchar temp_b = Original_Image->imageData[y*Original_Image->widthStep +3*x];
			uchar temp_g = Original_Image->imageData[y*Original_Image->widthStep +3*x+1];
			uchar temp_r = Original_Image->imageData[y*Original_Image->widthStep +3*x+2];

			int   index_id = cluster_index[y*Image_width + x];

			sum_b[index_id] += temp_b;
			sum_g[index_id] += temp_g;
			sum_r[index_id] += temp_r;

			num_color[index_id]++;

			if( x == 0 || x == Image_width - 1 || y == 0 || y == Image_height - 1 )
			{
				int index = cluster_index[y*Image_width + x];
				temp_index = index;
				patch[index].px.push_back(x);
				patch[index].py.push_back(y);

				vector2 temp;
				temp.set(x,y);
				global_boundary.push_back(temp);
			}
			else
			{
				int index      = cluster_index[y*Image_width + x];
				int index_up   = cluster_index[(y-1)*Image_width + x];
				int index_down = cluster_index[(y+1)*Image_width + x];
				if(temp_index != index)
				{
					patch[index].px.push_back(x);
					patch[index].py.push_back(y);
					patch[temp_index].px.push_back(x-1);
					patch[temp_index].py.push_back(y);

					vector2 temp;
					temp.set(x,y);
					global_boundary.push_back(temp);

					bool neighbor_flag = 0;
					int  patch_size    = patch[index].neighbor.size();

					for(int k = 0 ; k < patch_size ; k++)
					{
						if(patch[index].neighbor[k] == temp_index) 
						{
							neighbor_flag = 1;
						}
						if(neighbor_flag == 1) break;
					}
					if(neighbor_flag == 0) 
					{
						patch[index].neighbor.push_back(temp_index);
						patch[temp_index].neighbor.push_back(index);
					}
				}
				else if(temp_index != index_up || temp_index != index_down)
				{
					patch[temp_index].px.push_back(x);
					patch[temp_index].py.push_back(y);	

					if(temp_index != index_down)
					{
						vector2 temp;
						temp.set(x,y);
						global_boundary.push_back(temp);
					}
				}
				else ;

				if(temp_index != index ) temp_index = index;

			}
		}
	}

	for(int i = 0 ; i < patch_num ; i++)
	{
		patch[i].color_B = sum_b[i] / num_color[i];
		patch[i].color_G = sum_g[i] / num_color[i];
		patch[i].color_R = sum_r[i] / num_color[i];

		patch[i].number  = num_color[i];
	}

	sum_b.clear();
	sum_g.clear();
	sum_r.clear();

	num_color.clear();


}

void manager_image::SimplyGraph()
{
	vector<int> matching_table_src;
	vector<int> matching_table_dst;

	// 	for (int k = 0; k < 5 ; k++)
	// 	{
	for(int i = 0 ; i < patch_num ; i++)
	{
		int ori_color_R = patch[i].color_R;
		int ori_color_G = patch[i].color_G;
		int ori_color_B = patch[i].color_B;

		if(patch[i].path_id != -1)
		{
			int neighbor_size = patch[i].neighbor.size();
			for(int j = 0 ; j < neighbor_size ; j++)
			{

				int dst = patch[i].neighbor[j];

				bool mat_flag = 0;

				while(mat_flag == 0)
				{
					int old_dst = dst;
					int matching_size = matching_table_src.size();
					for(int m = 0 ; m < matching_size ; m++)
					{
						if(matching_table_src[m] == dst) dst = matching_table_dst[m];
					}
					if(dst == old_dst) mat_flag = 1;
					else               mat_flag = 0;
				}

				//int k = patch[i].neighbor[j];
				int k = dst;

				int com_color_R = patch[k].color_R;
				int com_color_G = patch[k].color_G;
				int com_color_B = patch[k].color_B;

				double color_difference = pow(float(ori_color_R - com_color_R) , 2 ) + pow(float(ori_color_G - com_color_G) , 2) + pow(float(ori_color_B - com_color_B) , 2);
				color_difference = pow(color_difference , 1);

				if(i == k || patch[dst].number <= 1 || patch[i].number <= 1 ) ;
				else
				{
					if(color_difference < 500.0 || /*patch[i].neighbor.size() == 1 &&*/ /*patch[dst].neighbor.size() == 1 && patch[dst].number < 300 ||*/ patch[i].number < 300)
					{

						int big_index;
						int small_index;

						if(patch[i].number >= patch[dst].number)
						{
							big_index   = i;
							small_index = dst;
						}
						else
						{
							big_index   = dst;
							small_index = i;  
						}

						matching_table_src.push_back(small_index);
						matching_table_dst.push_back(big_index);

						MergePatch(small_index ,big_index);

					}
					else ;
				}

				if(patch[i].number == 0) break;
			}
		}
		else ;

	}

	matching_table_src.clear();
	matching_table_dst.clear();

	patch.clear();

	CalculateEdge();

	//	}


}

void manager_image::MergePatch(int small_index , int big_index)
{
	int start_y = Image_height;
	int end_y   = 0;
	int start_x = Image_width;
	int end_x   = 0;

	int small_size = patch[small_index].px.size();
	for(int m = 0 ; m < small_size ; m++)
	{

		int x = patch[small_index].px[m];
		int y = patch[small_index].py[m];

		if(x > end_x)   end_x   = x;
		else                       ;

		if(x < start_x) start_x = x;
		else                       ;

		if(y > end_y)   end_y   = y;
		else                       ;

		if(y < start_y) start_y = y;
		else                       ;
	}

	for(int y = start_y ; y <= end_y ; y++)
	{
		for(int x = start_x ; x <= end_x ; x++)
		{
			int index = cluster_index[y*Image_width + x];

			if(index == small_index)
			{
				cluster_index[y*Image_width + x] = big_index;
			}
		}
	}
	patch[big_index].number += patch[small_index].number;
	patch[small_index].number = 0;
	patch[small_index].path_id = -1;
}

void manager_image::SortEdge()
{
	/*
	for(int i = 0 ; i < patch_num ; i++)
	{
	int patch_id = patch[i].path_id;

	if(patch_id != -1)
	{
	int patch_size = patch[i].px.size() - 1;

	for(int m = 0 ; m < patch_size ; m++)
	{
	int start_x = patch[i].px[m];
	int start_y = patch[i].py[m];

	float length , index , minimum;

	minimum = Image_width*Image_height*Image_width*Image_height;

	index = m+1;

	for(int n = m + 1 ; n < patch_size ; n++)
	{

	int end_x = patch[i].px[n];
	int end_y = patch[i].py[n];


	length = (end_x - start_x)*(end_x - start_x) + (end_y - start_y)*(end_y - start_y);

	if(length < minimum)
	{
	index = n;
	minimum = length;
	}

	if(length == 1) break;

	}


	int temp_x = patch[i].px[m+1];
	int temp_y = patch[i].py[m+1];

	patch[i].px[m+1] = patch[i].px[index];
	patch[i].py[m+1] = patch[i].py[index];

	patch[i].px[index] = temp_x;
	patch[i].py[index] = temp_y;

	}
	for(int iteration = 0 ; iteration < 5 ; iteration++)
	{
	int flag = 0;
	int index = patch[i].px.size() - 1;
	for(index ; index > 0 ; index--)
	{
	int min_length = (patch[i].px[index] - patch[i].px[0])*(patch[i].px[index] - patch[i].px[0]) + (patch[i].py[index] - patch[i].py[0])*(patch[i].py[index] - patch[i].py[0]);
	int min_index  = 0;
	for(int p = 1 ; p < index ; p++)
	{
	int length = (patch[i].px[index] - patch[i].px[p])*(patch[i].px[index] - patch[i].px[p]) + (patch[i].py[index] - patch[i].py[p])*(patch[i].py[index] - patch[i].py[p]);
	if(length < min_length)
	{
	min_length = length;
	min_index  = p;
	}
	}
	if(min_index == index - 1) flag++;
	else
	{
	flag = 0;
	for(int q = index ; q > min_index + 1 ; q--)
	{
	int temp = patch[i].px[q];
	patch[i].px[q] = patch[i].px[q-1];
	patch[i].px[q-1] = temp;

	temp = patch[i].py[q];
	patch[i].py[q] = patch[i].py[q-1];
	patch[i].py[q-1] = temp;
	}
	}
	if(flag == 20) break;
	}


	}

	}
	else ;

	}
	*/

	int boundary_size = global_boundary.size();
	for(int i = 0 ; i < boundary_size-1 ; i++)
	{

		float length , index , minimum;

		minimum = Image_width*Image_width + Image_height*Image_height;

		double start_x = global_boundary[i].x;
		double start_y = global_boundary[i].y;

		index = i+1;

		for(int j = i+1 ; j < boundary_size ; j++)
		{
			double end_x = global_boundary[j].x;
			double end_y = global_boundary[j].y;

			length = (end_x - start_x)*(end_x - start_x) + (end_y - start_y)*(end_y - start_y);

			if(length < minimum)
			{
				index = j;
				minimum = length;
			}

			if(length == 1) break;
		}

		double temp_x = global_boundary[i+1].x;
		double temp_y = global_boundary[i+1].y;

		global_boundary[i+1].x = global_boundary[index].x;
		global_boundary[i+1].y = global_boundary[index].y;

		global_boundary[index].x = temp_x;
		global_boundary[index].y = temp_y;
	}

	/*
	FILE *fp1;
	fp1 = fopen("polar.txt","w");
	for(int i = 0 ; i < boundary_size ; i++)
	{
	fprintf(fp1,"%f %f\n",global_boundary[i].x,global_boundary[i].y);
	}
	fclose(fp1);
	*/

}
void manager_image::CreateQuadMesh()
{

	Image_width  = Original_Image->width;
	Image_height = Original_Image->height;

	//  計算GridSize與QuadSize
	xGridSize = Image_width / gridRes;
	yGridSize = Image_height / gridRes;
	xQuadSize = (float)Image_width / (float)xGridSize;
	yQuadSize = (float)Image_height / (float)yGridSize;

	// vertex list
	vertexList.resize((xGridSize + 1) * (yGridSize + 1));

	//  儲存每一個頂點的座標
	for (int j = 0 ; j < xGridSize + 1 ; j++)
	{
		for (int k = 0 ; k < yGridSize + 1 ; k++)
		{
			int	vIdx = k * (xGridSize + 1) + j;
			vector2 pos((float)j * xQuadSize , (float)k * yQuadSize);

			vertexList[vIdx] = pos;
		}
	}

	vertexNum = vertexList.size();

	x_boundary.clear();
	y_boundary.clear();
	//  記錄是boundary的頂點id
	for(int i = 0 ; i < vertexNum ; i++)
	{
		if(vertexList[i].x == 0 || abs(vertexList[i].x - Image_width) < 1.0 )  x_boundary.push_back(i);
		if(vertexList[i].y == 0 || abs(vertexList[i].y - Image_height) < 1.0)  y_boundary.push_back(i);
	}

	// face list
	faceList.resize(xGridSize * yGridSize);

	for (int j = 0 ; j < xGridSize ; j++)
	{
		for (int k = 0 ; k < yGridSize ; k++)
		{
			int Idx = k * (xGridSize + 1) + j;

			faceList[k * xGridSize + j].x = Idx;
			faceList[k * xGridSize + j].y = Idx + 1;
			faceList[k * xGridSize + j].z = Idx + xGridSize + 2;
			faceList[k * xGridSize + j].w = Idx + xGridSize + 1;

		}
	}

	//  將頂點座標複製到其他儲存座標的陣列
	referenceList = vertexList;

	//  計算右下角的的頂點id
	rbIdx = (xGridSize + 1) * (yGridSize + 1) -1 ;


	Laplacian();
}

void manager_image::CreateTriangleMesh()
{

	/// triangular number points
	int pointNum = 4;	// 四個頂點

	int constrainLinesVertexNum = 0;	// constrain line 的 vertex
	for(int i = 0; i < constraintLines.size(); ++i)
		constrainLinesVertexNum += 2;
	pointNum += constrainLinesVertexNum;


	/// setup vertex
	double *points = new double[2*pointNum]();
	int *pointMarkers = new int[pointNum]();
	// points list:
	//          (x)                     (y)
	points[0] = 0.0;		    points[1] = 0.0;		    pointMarkers[0] = -1;	// point 0
	points[2] = 0.0;		    points[3] = Image_height-1;	pointMarkers[1] = 1;	// point 1
	points[4] = Image_width-1;  points[5] = Image_height-1;	pointMarkers[2] = 2;	// point 2
	points[6] = Image_width-1;  points[7] = 0.0;		    pointMarkers[3] = 3;	// point 3
	int pointIdx = 4; 

	// get constraint vertex
	for(int i = 0; i < constraintLines.size(); ++i) {
		for(int j = 0; j < 2; ++j) {
			points[pointIdx*2] = (constraintLines[i])[j].x;
			points[pointIdx*2+1] = (constraintLines[i])[j].y;
			pointMarkers[pointIdx] = pointIdx;
			++pointIdx;
		}
	}

	/// setup segment
	int segmentNum = 4;

	// get constraint line
	for(int i = 0; i < constraintLines.size(); ++i) {
		segmentNum++;
	}


	int *segments = new int[2*segmentNum]();
	int *segmentMarkers = new int[segmentNum]();
	// segments list:
	// start point id   end point id
	segments[0] = 0;	segments[1] = 1;	segmentMarkers[0] = -1;	// segment 0
	segments[2] = 1;	segments[3] = 2;	segmentMarkers[1] = 1;	// segment 1
	segments[4] = 2;	segments[5] = 3;	segmentMarkers[2] = 2;	// segment 2
	segments[6] = 3;	segments[7] = 0;	segmentMarkers[3] = 3;	// segment 3

	int segmentIdx = 4;

	int vertexCount = 4;

	// constraint line

	for (int i = 0 ; i < constraintLines.size() ; ++i)
	{

		segments[segmentIdx*2] = vertexCount ;
		segments[segmentIdx*2+1] = vertexCount + 1;
		segmentMarkers[segmentIdx] = segmentIdx;
		++segmentIdx;

		vertexCount +=2;
	}


	struct triangulateio in, out;
	memset(&in, 0, sizeof(triangulateio));
	memset(&out, 0, sizeof(triangulateio));

	/// Define input points
	in.numberofpoints = pointNum;
	in.pointlist = points;
	in.pointmarkerlist = pointMarkers;

	/// Define segments
	in.numberofsegments = segmentNum;
	in.segmentlist = segments;
	in.segmentmarkerlist = segmentMarkers;

	/// Define holes
	in.numberofholes = 0;

	// 	if(samplePointsNum == 0)
	//triangulate("pq30a300ze", &in, &out, NULL);
	triangulate("pq20a1600ze", &in, &out, NULL);
	// 	else
	// 		triangulate("pze", &in, &out, NULL);


	// Vertex
	vertexList.clear();
	for(int i = 0; i < out.numberofpoints; ++i) {
		int shift = i*2;
		vertexList.push_back(vector2(out.pointlist[shift], out.pointlist[shift+1]));
	}

	vertexNum = vertexList.size();

	detectedge.resize(constraintLines.size());

	edgeList.clear();
	for (int i = 0 ; i < out.numberofedges ; ++i)
	{
		int shift = i*2;
		edgeList.push_back(vector2(out.edgelist[shift] , out.edgelist[shift+1]));

		if(out.edgemarkerlist[i] >=4 )
		{
			vector2 edgeindex(out.edgelist[shift],out.edgelist[shift+1]);
			detectedge[out.edgemarkerlist[i]-4].push_back(edgeindex);
		}
	}

	// Boundary
	x_boundary.clear();
	y_boundary.clear();
	for (vector<vector2>::size_type i = 0 ; i < vertexList.size() ; ++i)
	{
		if(vertexList[i].y == 0 || vertexList[i].y == Image_height - 1)
			y_boundary.push_back(i);
		if(vertexList[i].x == 0 || vertexList[i].x == Image_width - 1)
			x_boundary.push_back(i);

		if(vertexList[i].x == Image_width - 1 && vertexList[i].y == Image_height - 1) rbIdx = i;
	}






	// Face, too late, need to update algorithm
	faceList.clear();
	//cout << "start face" << endl;
	for (int i = 0 ; i < out.numberoftriangles ; ++i)
	{
		int end = faceList.size();
		int shift = i*3;
		faceList.push_back(vector4(out.trianglelist[shift+0], out.trianglelist[shift+1], out.trianglelist[shift+2],0));
	}

	//cout << "end face" << endl;
	referenceList = vertexList;

	free(out.pointlist);
	free(out.pointmarkerlist);
	free(out.trianglelist);
	free(out.neighborlist);
	free(out.segmentlist);
	free(out.segmentmarkerlist);



}

void manager_image::SaliencyMeasure()
{
	SignificanceMap = new float[Image_width*Image_height];

	FastImportance imp = FastImportance(Original_Image);
	imp.calcEnergy(Original_Image);
	double m = 0;

	for (int y = 0; y < Image_height; y++)
	{
		for (int x = 0; x < Image_width; x++)
		{
			double s = imp.E[y][x];
			if(s > m) m = s;
		}
	}

	Saliency_Map = cvCreateImage(cvSize(Image_width,Image_height),IPL_DEPTH_8U,3);

	for (int y = 0; y < Image_height; y++)
	{
		for (int x = 0; x < Image_width; x++)
		{
			int shift = y * Image_width + x;
			SignificanceMap[shift]=(imp.E[y][x]/m);

			vector3 _color = GrayToColor(SignificanceMap[shift]);

			/*			Saliency_Map->imageData[y*Saliency_Map->widthStep + x ] = 255*SignificanceMap[shift];*/
			Saliency_Map->imageData[y*Saliency_Map->widthStep + 3*x + 0] = 255*SignificanceMap[shift];
			Saliency_Map->imageData[y*Saliency_Map->widthStep + 3*x + 1] = 255*SignificanceMap[shift];
			Saliency_Map->imageData[y*Saliency_Map->widthStep + 3*x + 2] = 255*SignificanceMap[shift];
			// 			Saliency_Map->imageData[y*Saliency_Map->widthStep + 3*x + 0] = 255*_color.z;
			// 			Saliency_Map->imageData[y*Saliency_Map->widthStep + 3*x + 1] = 255*_color.y;
			// 			Saliency_Map->imageData[y*Saliency_Map->widthStep + 3*x + 2] = 255*_color.x;

		}
	}

	// 	cvNamedWindow("111",1);
	// 	cvMoveWindow("111",100,100);
	// 	cvShowImage("111",Saliency_Map);
}

void manager_image::Saliency()
{
	printf("Saliency.......\n");

	IplImage *Lab,*Lab_1,*Lab_2,*Lab_3;
	CvSize ImageSize1 = cvSize(Image_width,Image_height);

	CvSize ImageSize2,ImageSize3,ImageSize4,ImageSize5;

	IplImage *Image_scale_1,*Image_scale_2,*Image_scale_3,*Image_scale_4;

	double scale_1 = 0.8;
	double scale_2 = 0.5;
	double scale_3 = 0.3;

	int width , height;

	if(Image_width >= Image_height && Image_width > 250)
	{
		width  = 250;
		height = Image_height*250/Image_width;


	}
	else if(Image_width < Image_height && Image_height > 250)
	{
		width  = Image_width*250/Image_height;
		height = 250;
	}
	else 
	{
		width  = Image_width;
		height = Image_height;
	};


	ImageSize2 = cvSize(width,height);
	ImageSize3 = cvSize(int(width*scale_1),int(scale_1*height));
	ImageSize4 = cvSize(int(width*scale_2),int(scale_2*height));
	ImageSize5 = cvSize(int(width*scale_3),int(scale_3*height));

	Image_scale_1 = cvCreateImage(ImageSize2,IPL_DEPTH_8U,3); 
	Image_scale_2 = cvCreateImage(ImageSize3,IPL_DEPTH_8U,3); 
	Image_scale_3 = cvCreateImage(ImageSize4,IPL_DEPTH_8U,3); 
	Image_scale_4 = cvCreateImage(ImageSize5,IPL_DEPTH_8U,3); 

	cvResize(Original_Image,Image_scale_1);
	cvResize(Original_Image,Image_scale_2);
	cvResize(Original_Image,Image_scale_3);
	cvResize(Original_Image,Image_scale_4);

	Lab   = cvCreateImage(ImageSize2,IPL_DEPTH_8U,3); 
	Lab_1 = cvCreateImage(ImageSize3,IPL_DEPTH_8U,3); 
	Lab_2 = cvCreateImage(ImageSize4,IPL_DEPTH_8U,3); 
	Lab_3 = cvCreateImage(ImageSize5,IPL_DEPTH_8U,3); 

	cvCvtColor( Image_scale_1, Lab  , CV_BGR2Lab );
	cvCvtColor( Image_scale_2, Lab_1, CV_BGR2Lab );
	cvCvtColor( Image_scale_3, Lab_2, CV_BGR2Lab );
	cvCvtColor( Image_scale_4, Lab_3, CV_BGR2Lab );

	double time_1 = clock();

	printf("Scale level 1 ......\n");
	IplImage *temp   = SaliencyMap(Lab);
	printf("Scale level 2 ......\n");
	IplImage *temp_1 = SaliencyMap(Lab_1);
	printf("Scale level 3 ......\n");
	IplImage *temp_2 = SaliencyMap(Lab_2);
	printf("Scale level 4 ......\n");
	IplImage *temp_3 = SaliencyMap(Lab_3);

	IplImage *Saliency_map,*Saliency_map_1,*Saliency_map_2,*Saliency_map_3;

	Saliency_map   = cvCreateImage(ImageSize2,IPL_DEPTH_8U,1); 
	Saliency_map_1 = cvCreateImage(ImageSize2,IPL_DEPTH_8U,1); 
	Saliency_map_2 = cvCreateImage(ImageSize2,IPL_DEPTH_8U,1); 
	Saliency_map_3 = cvCreateImage(ImageSize2,IPL_DEPTH_8U,1); 

	cvResize(temp,Saliency_map);
	cvResize(temp_1,Saliency_map_1);
	cvResize(temp_2,Saliency_map_2);
	cvResize(temp_3,Saliency_map_3);

	IplImage *Saliency_Sum   = cvCreateImage(ImageSize2,IPL_DEPTH_8U,1);

	vector<int> s_x;
	vector<int> s_y;

	for(int j = 0 ; j < ImageSize2.height ; j++)
	{
		for(int i = 0 ; i < ImageSize2.width ; i++)
		{
			uchar s1 = Saliency_map->imageData[j*Saliency_map->widthStep + i];
			uchar s2 = Saliency_map_1->imageData[j*Saliency_map_1->widthStep + i];
			uchar s3 = Saliency_map_2->imageData[j*Saliency_map_2->widthStep + i];
			uchar s4 = Saliency_map_3->imageData[j*Saliency_map_3->widthStep + i];

			Saliency_Sum->imageData[j*Saliency_Sum->widthStep + i]    = (s1+s2+s3+s4)/4;

			double s = double(s1+s2+s3+s4)/(4*256);
			if(s > 0.8)
			{
				s_x.push_back(i);
				s_y.push_back(j);
			}

		}
	}

	int s_r_size = s_x.size();

	for(int j = 0 ; j < ImageSize2.height ; j++)
	{
		for(int i = 0 ; i < ImageSize2.width ; i++)
		{
			uchar s = Saliency_Sum->imageData[j*Saliency_Sum->widthStep + i];

			double ratio = double(s) / 256;
			if(ratio < 0.8)
			{
				int distance;

				int d_max = ImageSize2.height*ImageSize2.height + ImageSize2.width*ImageSize2.width;
				int d_min = d_max;

				for(int k = 0 ; k < s_r_size ; k++)
				{
					int x = s_x[k];
					int y = s_y[k];

					distance =  (x-i)*(x-i) + (y-j)*(y-j);

					if(distance < d_min) d_min = distance;

				}

				double new_s = ratio*(1-d_min/d_max);
				Saliency_Sum->imageData[j*Saliency_Sum->widthStep + i] = new_s*255;
			}
		}
	}

	IplImage *Saliency_temp   = cvCreateImage(ImageSize1,IPL_DEPTH_8U,1);
	cvResize(Saliency_Sum,Saliency_temp);

	Saliency_Map   = cvCreateImage(ImageSize1,IPL_DEPTH_8U,3);

	for(int j = 0 ; j < Image_height ; j++)
	{
		for(int i = 0 ; i < Image_width ; i++)
		{
			uchar s = Saliency_temp->imageData[j*Saliency_temp->widthStep + i];

			Saliency_Map->imageData[j*Saliency_Map->widthStep + 3*i]     = s;
			Saliency_Map->imageData[j*Saliency_Map->widthStep + 3*i + 1] = s;
			Saliency_Map->imageData[j*Saliency_Map->widthStep + 3*i + 2] = s;
		}
	}

	printf("total time:%f second....\n",(clock()-time_1)/1000);

	//PatchSaliency();
}
IplImage *manager_image::SaliencyMap(IplImage *Image)
{
	vector<float> Lab_L;
	vector<float> Lab_a;
	vector<float> Lab_b;

	uchar L_max = Image->imageData[0];
	uchar L_min = Image->imageData[0];

	uchar a_max = Image->imageData[1];
	uchar a_min = Image->imageData[1];

	uchar b_max = Image->imageData[2];
	uchar b_min = Image->imageData[2];

	int Image_width  = Image->width;
	int Image_height = Image->height;

	for(int j = 0 ; j < Image_height ; j++)
	{
		for(int i = 0 ; i < Image_width ; i++)
		{
			uchar color_L = Image->imageData[j*Image->widthStep + 3*i ];
			uchar color_a = Image->imageData[j*Image->widthStep + 3*i +1];
			uchar color_b = Image->imageData[j*Image->widthStep + 3*i +2];

			if(color_L > L_max) L_max = color_L;
			else                               ;

			if(color_L < L_min) L_min = color_L;
			else                               ;

			if(color_a > a_max) a_max = color_a;
			else                               ;

			if(color_a < a_min) a_min = color_a;
			else                               ;

			if(color_b > b_max) b_max = color_b;
			else                               ;

			if(color_b < b_min) b_min = color_b;
			else                               ;	

			Lab_L.push_back(color_L);
			Lab_a.push_back(color_a);
			Lab_b.push_back(color_b);
		}
	}

	uchar d_L = L_max - L_min;
	uchar d_a = a_max - a_min;
	uchar d_b = b_max - b_min;

	vector<double> saliency;
	vector<int>    p_num;


	for(int i = 0 ; i < Image_width*Image_height ; i++)
	{
		Lab_L[i] = float(Lab_L[i] - L_min) / (1+d_L);
		Lab_a[i] = float(Lab_a[i] - a_min) / (1+d_a);
		Lab_b[i] = float(Lab_b[i] - b_min) / (1+d_b);

		saliency.push_back(0.0);
		p_num.push_back(1);
	}

	s_table temp_table;

	vector<s_table> similarity_table;
	vector<double>  difference;

	int index = 0;

	for(int j = 3 ; j <= Image_height - 4 ; j+=4)
	{
		for(int i = 3 ; i <= Image_width - 4 ; i+=4)
		{
			similarity_table.push_back(temp_table);
			similarity_table[index].id = index;
			similarity_table[index].x  = i;
			similarity_table[index].y  = j;
			index++;
			difference.push_back(0);
		}
	}



	int K = 64;

	double s_max = 0.0;
	double s_min = 1;

	double d_p = 0.0f;
	double d_c = 0.0f;

	double sum_difference = 0.0f;

	int similarity_table_size = similarity_table.size();

	for(int i = 0 ; i < similarity_table_size ; i++)
	{
		similarity_table[i].d_max_id = 0;

		for(int j = i+1 ; j < similarity_table_size ; j++)
		{

			int x  = similarity_table[i].x;
			int y  = similarity_table[i].y;

			int x1 = similarity_table[j].x;
			int y1 = similarity_table[j].y;

			sum_difference = 0.0f;
			d_c            = 0.0f;

			for(int m = -3 ; m <=  3 ; m++)
			{
				for(int n = -3 ; n <=  3 ; n++)
				{
					int index_1 = (y+m) *Image_width + (x+n);
					int index_2 = (y1+m)*Image_width + (x1+n);

					double L_1 = Lab_L[index_1];
					double L_2 = Lab_L[index_2];

					double a_1 = Lab_a[index_1];
					double a_2 = Lab_a[index_2];

					double b_1 = Lab_b[index_1];
					double b_2 = Lab_b[index_2];


					double sum = (L_1 - L_2)*(L_1 - L_2) + (a_1 - a_2)*(a_1 - a_2) + (b_1 - b_2)*(b_1 - b_2);
					double c_d = abs(L_1 - L_2) + abs(a_1 - a_2) + abs(b_1 - b_2);
					d_c += sqrt(sum);
					//d_c += sum;

				}
			}



			//d_c  = pow(d_c,0.5);
			d_p  = sqrt((double)(x-x1)*(x-x1) + (double)(y-y1)*(y-y1));

			sum_difference = d_c / (1 + 3*d_p);

			//difference[i] += sum_difference;
			//difference[j] += sum_difference;

			int difference_size = similarity_table[i].difference.size();
			if(difference_size < K)
			{
				similarity_table[i].difference.push_back(sum_difference);
				if(difference_size == K - 1 )
				{
					similarity_table[i].d_max_id = 0;
					for(int p = 0 ; p < K ; p++) 
					{
						if(similarity_table[i].difference[p] > similarity_table[i].difference[similarity_table[i].d_max_id])
							similarity_table[i].d_max_id = p;
					}
				}
			}
			else
			{
				if(sum_difference < similarity_table[i].difference[similarity_table[i].d_max_id])
				{
					similarity_table[i].difference[similarity_table[i].d_max_id] = similarity_table[i].difference[K-1];
					similarity_table[i].difference.pop_back();
					similarity_table[i].difference.push_back(sum_difference);
					similarity_table[i].d_max_id = K-1;
					for(int p = 0 ; p < K ; p++) 
					{
						if(similarity_table[i].difference[p] > similarity_table[i].difference[similarity_table[i].d_max_id])
							similarity_table[i].d_max_id = p;
					}

				}
			}

			difference_size = similarity_table[j].difference.size();
			if(difference_size < K)
			{
				similarity_table[j].difference.push_back(sum_difference);
				if(difference_size == K - 1 )
				{
					similarity_table[j].d_max_id = 0;
					for(int p = 0 ; p < K ; p++) 
					{
						if(similarity_table[j].difference[p] > similarity_table[j].difference[similarity_table[j].d_max_id])
							similarity_table[j].d_max_id = p;
					}
				}
			}
			else
			{
				if(sum_difference < similarity_table[j].difference[similarity_table[j].d_max_id])
				{
					similarity_table[j].difference[similarity_table[j].d_max_id] = similarity_table[j].difference[K-1];
					similarity_table[j].difference.pop_back();
					similarity_table[j].difference.push_back(sum_difference);
					similarity_table[j].d_max_id = K-1;
					for(int p = 0 ; p < K ; p++) 
					{
						if(similarity_table[j].difference[p] > similarity_table[j].difference[similarity_table[j].d_max_id])
							similarity_table[j].d_max_id = p;
					}

				}
			}

		}


		double sum_d = 0.0;

		int difference_size = difference.size();

		for(int p = 0 ; p < K ; p++) sum_d += similarity_table[i].difference[p];

		double s = 1 - pow(2.71,-sum_d/K);
		//double s = 1 - pow(2.71,-difference[i]/difference_size);

		int x = similarity_table[i].x;
		int y = similarity_table[i].y;

		saliency[y*Image_width+ x] = s;

		if(s > s_max) s_max = s;
		if(s < s_min) s_min = s;

		/*
		for(int m = y - 3 ; m <= y + 3 ; m++)
		{
		for(int n = x - 3 ; n <= x + 3 ; n++)
		{
		p_num[m*Image_width + n]++;
		saliency[m*Image_width + n] += s;
		}
		}
		*/
	}


	for(int i = 0 ; i < similarity_table_size ; i++)
	{
		int x = similarity_table[i].x;
		int y = similarity_table[i].y;

		if((x+8) > Image_width || (y+8) > Image_height)
		{

		}
		else
		{
			double s1 = saliency[y*Image_width + x];
			double s2 = saliency[(y+4)*Image_width + x];
			double s3 = saliency[y*Image_width + x+4];
			double s4 = saliency[(y+4)*Image_width + x+4]; 


			for(int m = x ; m <= x+4 ; m++)
			{
				for(int n = y ; n <= y+4 ; n++)
				{
					double s = (s1*(x+4-m)*(y+4-n) + s2*(x+4-m)*(n-y) + s3*(m-x)*(y+4-n) + s4*(m-x)*(n-y)) / 16;  
					saliency[n*Image_width+m] = s;
				}
			}

		}

	}

	int end_x = similarity_table[similarity_table_size-1].x;
	int end_y = similarity_table[similarity_table_size-1].y;

	for(int x = 3 ; x < end_x ; x+=4)
	{
		int y = end_y; 
		double s1 = saliency[y*Image_width + x];
		double s2 = s1;
		double s3 = saliency[y*Image_width + x+4];
		double s4 = s3; 

		for(int m = x ; m <= x+4 ; m++)
		{
			for(int n = y ; n < Image_height ; n++)
			{
				double s = (s1*(x+4-m)*(Image_height-n) + s2*(x+4-m)*(n-y) + s3*(m-x)*(Image_height-n) + s4*(m-x)*(n-y)) / (4*(Image_height-y));  
				saliency[n*Image_width+m] = s;
			}
		}

		y = 3; 
		s1 = saliency[y*Image_width + x];
		s2 = s1;
		s3 = saliency[y*Image_width + x+4];
		s4 = s3; 

		for(int m = x ; m <= x+4 ; m++)
		{
			for(int n = 0 ; n <= y ; n++)
			{
				double s = (s1*(x+4-m)*(y+4-n) + s2*(x+4-m)*(n-y) + s3*(m-x)*(y+4-n) + s4*(m-x)*(n-y)) / 16;  
				saliency[n*Image_width+m] = s;
			}
		}

		if(x == 3)
		{ 
			double s1 = saliency[y*Image_width + x];
			double s2 = s1;
			double s3 = saliency[0*Image_width + x];
			double s4 = s3; 

			for(int m = 0 ; m <= 3 ; m++)
			{
				for(int n = 0 ; n <= 3 ; n++)
				{
					double s = (s1*(x+4-m)*(y+4-n) + s2*(x+4-m)*(n-y) + s3*(m-x)*(y+4-n) + s4*(m-x)*(n-y)) / 16;  
					saliency[n*Image_width+m] = s;
				}
			}
		}
		if(x == end_x - 4)
		{
			double s1 = saliency[y*Image_width + x + 4 ];
			double s2 = s1;
			double s3 = saliency[0*Image_width + x + 4 ];
			double s4 = s3; 

			for(int m = end_x ; m < Image_width ; m++)
			{
				for(int n = 0 ; n <= 3 ; n++)
				{
					double s = (s1*(Image_width-m)*(y+4-n) + s2*(Image_width-m)*(n-y) + s3*(m-x)*(y+4-n) + s4*(m-x)*(n-y)) / (4*(Image_width-x));  
					saliency[n*Image_width+m] = s;
				}
			}
		}
	}

	for(int y = 3 ; y < end_y ; y+=4)
	{
		int x = end_x; 
		double s1 = saliency[y*Image_width + x];
		double s2 = saliency[(y+4)*Image_width + x];
		double s3 = s1;
		double s4 = s2; 

		for(int m = x ; m < Image_width ; m++)
		{
			for(int n = y ; n <= y+4 ; n++)
			{
				double s = (s1*(Image_width-m)*(y+4-n) + s2*(Image_width-m)*(n-y) + s3*(m-x)*(y+4-n) + s4*(m-x)*(n-y)) / (4*(Image_width-x));  
				saliency[n*Image_width+m] = s;
			}
		}

		x = 3; 
		s1 = saliency[y*Image_width + x];
		s2 = saliency[(y+4)*Image_width + x];
		s3 = s1;
		s4 = s2; 

		for(int m = 0 ; m <= x ; m++)
		{
			for(int n = y ; n <= y+4 ; n++)
			{
				double s = (s1*(x+4-m)*(y+4-n) + s2*(x+4-m)*(n-y) + s3*(m-x)*(y+4-n) + s4*(m-x)*(n-y)) / 16;  
				saliency[n*Image_width+m] = s;
			}
		}

		if(y == end_y - 4)
		{

			x = 3;
			double s1 = saliency[(y+4)*Image_width + x];
			double s2 = s1;
			double s3 = saliency[(y+4)*Image_width + 0];
			double s4 = s3; 

			for(int m = 0 ; m <= x ; m++)
			{
				for(int n = end_y ; n < Image_height ; n++)
				{
					double s = (s1*(x+4-m)*(Image_height-n) + s2*(x+4-m)*(n-y) + s3*(m-x)*(Image_height-n) + s4*(m-x)*(n-y)) / (4*(Image_height-y));  
					saliency[n*Image_width+m] = s;
				}
			}


			x = end_x;
			s1 = saliency[(y+4)*Image_width + x];
			s2 = s1;
			s3 = saliency[(y+4)*Image_width + Image_width-1];
			s4 = s3; 

			for(int m = end_x ; m < Image_width ; m++)
			{
				for(int n = end_y ; n < Image_height ; n++)
				{
					double s = (s1*(Image_width-m)*(Image_height-n) + s2*(Image_width-m)*(n-y) + s3*(m-x)*(Image_height-n) + s4*(m-x)*(n-y)) / ((Image_width-x)*(Image_height-y));  
					saliency[n*Image_width+m] = s;
				}
			}


		}
	}

	IplImage *importance_map;
	CvSize ImageSize2 = cvSize(Image_width,Image_height);
	importance_map = cvCreateImage(ImageSize2,IPL_DEPTH_8U,1); 

	double d_s = s_max - s_min;

	for(int j = 0 ; j < Image_height ; j++)
	{
		for(int i = 0 ; i < Image_width ; i++)
		{
			/*
			double s ;//= saliency[j*Image_width + i] / p_num[j*Image_width + i];
			if(p_num[j*Image_width + i] == 1) s = 0;
			else                              s = saliency[j*Image_width + i] / (p_num[j*Image_width + i] - 1);
			*/
			double s = saliency[j*Image_width + i];

			s = (s - s_min)*255/d_s; 

			if(s > 255.0) s = 255;
			if(s < 0)     s = 0;
			importance_map->imageData[j*importance_map->widthStep + i] = (int)s; 
		}
	}

	Lab_L.clear();
	Lab_a.clear();
	Lab_b.clear();
	saliency.clear();
	p_num.clear();
	similarity_table.clear();
	similarity_table.clear();


	return importance_map;
}

void manager_image::MeshSaliency()
{
	if(triangle)
	{
		faceflexibleList.resize(faceList.size());

		int *num ;

		num = new int[patch_num];

		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int left = Image_width, right = 0;
			int up = Image_height, down = 0;
			int Idx[3] = {faceList[i].x,faceList[i].y,faceList[i].z};

			for(unsigned k = 0; k < 3; ++k) {
				if(vertexList[Idx[k]].x < left)		left = vertexList[Idx[k]].x;
				if(vertexList[Idx[k]].x > right)	right = vertexList[Idx[k]].x;
				if(vertexList[Idx[k]].y < up)		up = vertexList[Idx[k]].y;
				if(vertexList[Idx[k]].y > down)		down = vertexList[Idx[k]].y;
			}

			if (right > Image_width - 1)		right = Image_width - 1;
			if (down > Image_height - 1)		down = Image_height - 1;

			int count = 0;
			faceflexibleList[i] = 0.0f;

			for(int j = 0 ; j < patch_num ; j++) num[j] = 0;

			int num_max = 0;
			int max_index = -1;

			for(unsigned m = left; m <= right; ++m) {
				for(unsigned n = up; n <= down; ++n) {
					// 檢查是否落於triangle內
					if(IsInTriangle(vertexList[Idx[0]], vertexList[Idx[1]], vertexList[Idx[2]], vector2(m, n)) == true) {
						int shift = n * Image_width + m;

						int index = cluster_index[n*Image_width + m];
						num[index]++;

						if(num[index] > num_max)
						{
							num_max = num[index];
							max_index = index;
						}
						++count;
						uchar saliency = Saliency_Map->imageData[n*Saliency_Map->widthStep + 3*m];

						faceflexibleList[i] += (saliency);
						//faceflexibleList[i] += SignificanceMap[shift];
					}
				}
			}

			if(max_index != -1) patch[max_index].triangle.push_back(i);
			//cout << "significant count: " << count << endl;
			// 			if (count > 0)
			// 			{
			// 				faceflexibleList[i] /= (float)count;
			// 			}
			else
			{
				faceflexibleList[i] = 0.0f;
			}
		}	

	}
	else
	{
		faceflexibleList.resize(faceList.size());

		int *num ;

		num = new int[patch_num];

		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};
			int left  = referenceList[vList[0]].x;
			int right = referenceList[vList[2]].x;
			int up    = referenceList[vList[0]].y;
			int down  = referenceList[vList[2]].y;


			int count = 0;
			faceflexibleList[i] = 0.0f;

			for(int j = 0 ; j < patch_num ; j++) num[j] = 0;

			int num_max = 0;
			int max_index = -1;

			for(unsigned m = left; m < right; ++m) 
			{
				for(unsigned n = up; n < down; ++n) 
				{

					int shift = n * Image_width + m;

					int index = cluster_index[n*Image_width + m];
					num[index]++;

					if(num[index] > num_max)
					{
						num_max = num[index];
						max_index = index;
					}
					++count;
					uchar saliency = Saliency_Map->imageData[n*Saliency_Map->widthStep + 3*m];
					faceflexibleList[i] += (saliency);
					//faceflexibleList[i] += SignificanceMap[shift];
				}
			}

			if(max_index != -1) patch[max_index].quad.push_back(i);
			//cout << "significant count: " << count << endl;
			// 			if (count > 0)
			// 			{
			// 				faceflexibleList[i] /= (float)count;
			// 			}
			else
			{
				faceflexibleList[i] = 0.0f;
			}
		}	
		// 		faceflexibleList.resize(faceList.size());
		// 		for (int j = 0 ; j < Image_width ; j++)
		// 		{
		// 			for (int k = 0 ; k < Image_height ; k++)
		// 			{
		// 				int xIdx = j / xQuadSize;
		// 				int yIdx = k / yQuadSize;
		// 				int qIdx = yIdx * xGridSize + xIdx;
		// 
		// 				int shift = k * Image_width + j;
		// 				uchar saliency = Saliency_Map->imageData[k*Saliency_Map->widthStep + 3*j];
		// 				//faceflexibleList[qIdx] += saliency;
		// 				faceflexibleList[qIdx] += SignificanceMap[shift];
		// 			}
		// 		}
	}


	// 	vector<float> meanXgrid;
	// 	vector<float> meanYgrid;
	//  	saliency2.resize(faceflexibleList.size());
	// 	meanXgrid.resize(yGridSize);
	// 	meanYgrid.resize(xGridSize);
	// 	float meanX = 0.0;
	// 	float meanY = 0.0;
	// 	int number = 0;
	//     for (int j = 0 ; j < yGridSize ; ++j)
	// 	{	
	// 		float sumX = 0.0 ;
	// 		for (int i = 0 ; i < xGridSize ; i++)
	// 		{
	// 			sumX += faceflexibleList [j+i*yGridSize];  
	// 		}
	// 		meanXgrid[j] = sumX / xGridSize;
	// //		printf ("sumXgrid = %f \n", sumXgrid[j]);
	// 	}
	// 	for (int j = 0 ; j < xGridSize ; ++j)
	// 	{	
	// 		float sumY = 0.0 ;
	// 		for (int i = 0 ; i < yGridSize ; i++)
	// 		{
	// 			sumY += faceflexibleList [j*yGridSize+i];  
	// 		}
	// 		meanYgrid[j] = sumY / yGridSize;
	// //		printf ("sumYgrid = %f \n", sumYgrid[j]);
	// 	}
	// 
	// 	for (int j = 0 ; j < xGridSize ; ++j)
	// 	{
	// 		for (int k = 0 ; k < yGridSize ; k++)
	// 		{
	//             saliency2[number++] = (meanXgrid[k] + meanYgrid[j])/2; 
	// //			printf ("saliency2 = %f \n", saliency2[number-1]);
	// 		}
	// 	}

	float max_differ = 0.0;
	for(int i = 0 ; i < faceflexibleList.size() ; ++i)
	{
		if(faceflexibleList[i] > max_differ)  max_differ = faceflexibleList[i];
	}

	FaceSelect.resize(faceList.size());

	for(int i = 0 ; i < faceflexibleList.size() ; ++i)
	{
		faceflexibleList[i] /= max_differ;
		//faceflexibleList[i] = faceflexibleList[i]*0.9 + 0.1;

		FaceSelect[i] = false;
	}
}

void manager_image::MeshSaliency2()
{
	if(triangle)
	{
		saliency2.resize(faceList.size());

		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int left = Image_width, right = 0;
			int up = Image_height, down = 0;
			int Idx[3] = {faceList[i].x,faceList[i].y,faceList[i].z};

			for(unsigned k = 0; k < 3; ++k) {
				if(vertexList[Idx[k]].x < left)		left = vertexList[Idx[k]].x;
				if(vertexList[Idx[k]].x > right)	right = vertexList[Idx[k]].x;
				if(vertexList[Idx[k]].y < up)		up = vertexList[Idx[k]].y;
				if(vertexList[Idx[k]].y > down)		down = vertexList[Idx[k]].y;
			}

			if (right > Image_width - 1)		right = Image_width - 1;
			if (down > Image_height - 1)		down = Image_height - 1;

			int count = 0;
			saliency2[i] = 0.0f;

			for(unsigned m = left; m <= right; ++m) {
				for(unsigned n = up; n <= down; ++n) {
					// 檢查是否落於triangle內
					if(IsInTriangle(vertexList[Idx[0]], vertexList[Idx[1]], vertexList[Idx[2]], vector2(m, n)) == true) {
						int shift = n * Image_width + m;
						++count;
						saliency2[i] += SignificanceMap[shift];
					}
				}
			}
			//cout << "significant count: " << count << endl;
			if (count > 0)
			{
				saliency2[i] /= (float)count;
			}
			else
			{
				saliency2[i] = 0.0f;
			}
		}	

	}
	else
	{
		saliency2.resize(faceList.size());
		for (int j = 0 ; j < Image_width ; j++)
		{
			for (int k = 0 ; k < Image_height ; k++)
			{
				int xIdx = j / xQuadSize;
				int yIdx = k / yQuadSize;
				int qIdx = yIdx * xGridSize + xIdx;

				int shift = k * Image_width + j;
				saliency2[qIdx] += SignificanceMap[shift];
			}
		}
	}


	vector<float> meanXgrid;
	vector<float> meanYgrid;
	saliency2.resize(faceflexibleList.size());
	meanXgrid.resize(yGridSize);
	meanYgrid.resize(xGridSize);
	float meanX = 0.0;
	float meanY = 0.0;
	int number = 0;
	for (int j = 0 ; j < yGridSize ; ++j)
	{	
		float sumX = 0.0 ;
		for (int i = 0 ; i < xGridSize ; i++)
		{
			sumX += faceflexibleList [j+i*yGridSize];  
		}
		meanXgrid[j] = sumX / xGridSize;
		//		printf ("sumXgrid = %f \n", sumXgrid[j]);
	}
	for (int j = 0 ; j < xGridSize ; ++j)
	{	
		float sumY = 0.0 ;
		for (int i = 0 ; i < yGridSize ; i++)
		{
			sumY += faceflexibleList [j*yGridSize+i];  
		}
		meanYgrid[j] = sumY / yGridSize;
		//		printf ("sumYgrid = %f \n", sumYgrid[j]);
	}

	for (int j = 0 ; j < xGridSize ; ++j)
	{
		for (int k = 0 ; k < yGridSize ; k++)
		{
			saliency2[number++] = (meanXgrid[k] + meanYgrid[j])/2; 
			//printf ("saliency2 = %f \n", saliency2[number-1]);
		}
	}



	float max_differ = 0.0;
	for(int i = 0 ; i < saliency2.size() ; ++i)
	{
		if(saliency2[i] > max_differ)  max_differ = saliency2[i];
	}

	FaceSelect.resize(faceList.size());

	for(int i = 0 ; i < saliency2.size() ; ++i)
	{
		saliency2[i] /= max_differ;
		//		saliency2[i] = saliency2[i]*0.9 + 0.1;

		FaceSelect[i] = false;
		//printf("saliency2[%d] = %f \n" , i , saliency2[i]);
	}
}

void manager_image::MaskSaliency()
{


	// 	faceflexibleList.resize(faceList.size());
	// 
	// 	for(int i = 0 ; i < faceList.size() ; ++i)
	// 	{
	// 		if(cluster_index[i] == 1) faceflexibleList[i] = 0.6;
	// 		else                      faceflexibleList[i] = 0.4;
	// 
	// 	}
	faceflexibleList.resize(faceList.size());

	int imnumber = 1;
	int nonimnumber = 1;
	for(int i = 0 ; i < faceList.size() ; ++i)
	{
		if(cluster_index[i] == 1)
		{
			importantpatch +=faceflexibleList[i];
			imnumber++;
		}


		else
		{
			nonimportantpatch+=faceflexibleList[i];
			nonimnumber++;
		}
	}
	importantpatch = importantpatch / imnumber; 
	nonimportantpatch = nonimportantpatch / nonimnumber;
	// 	float sumallmasksaliency = importantpatch+nonimportantpatch;
	// 	importantpatch = importantpatch / sumallmasksaliency;
	// 	nonimportantpatch = nonimportantpatch / sumallmasksaliency;
	printf("(important, nonimportant) = (%f , %f)\n" , importantpatch,nonimportantpatch);

}

bool manager_image::IsInTriangle(vector2 &a, vector2 &b, vector2 &c, vector2 &p)
{
	// compute vectors
	vector2 v0 = c-a;
	vector2 v1 = b-a;
	vector2 v2 = p-a;
	// compute dot products
	float dot00 = DotProduct(v0, v0);
	float dot01 = DotProduct(v0, v1);
	float dot02 = DotProduct(v0, v2);
	float dot11 = DotProduct(v1, v1);
	float dot12 = DotProduct(v1, v2);
	// compute barycentric coordinates
	float invDenom = 1/(dot00*dot11-dot01*dot01);
	float u = (dot11*dot02-dot01*dot12)*invDenom;
	float v = (dot00*dot12-dot01*dot02)*invDenom;
	// check if point is in triangle
	return (u>0) && (v>0) && (u+v<1);
}

bool manager_image::IsInQuad(vector2 &a, vector2 &b, vector2 &c, vector2 &d, vector2 &p)
{
	if(p.x >= a.x && p.x <= b.x && p.y >= a.y && p.y <= c.y) return true;
	else                                                     return false;
}

vector3 manager_image::GrayToColor(float gray)
{
	// gray為灰階，result彩色
	float weight = gray;

	// result.x為R，result.y為G，result.z為B

	/*    vector3 result(weight , weight , weight);*/
	vector3 result(0.0f , 0.0f , 0.0f);

	if (weight > 1.0f)		return vector3(1.0f , 0.0f , 0.0f);
	if (weight < 0.0f)		return vector3(0.0f , 0.0f , 1.0f);

	if (weight < 0.33333f)
	{
		result.z = 1.0f - weight * 3.0f;
		result.y = weight * 3.0f;
	}
	else if (weight < 0.66666f)
	{
		result.x = (weight - 0.33333f) * 3.0f;
		result.y = 1.0f;
	}
	else
	{
		result.x = 1.0f;
		result.y = 1.0f - (weight - 0.66666f) * 3.0f;
	}

	return result;
}

void manager_image::ImageResize(vector2 move,int situation)
{

	int ver_size = vertexList.size();
	for(int i = 0 ; i < x_boundary.size() ; i++)
	{
		int index = x_boundary[i];
		if(referenceList[index].x != 0)
		{
			vertexList[index].x = vertexList[index].x + move.x;
			vertexList[index].y = vertexList[index].y + move.y;
		}
	}

	bool convergence = 0;
	bool moreFlips   = 1;
	int  iterationNum = 0;

	int flip_count = 0;

	flippedConstraintList.clear();

	if(triangle) updateedgeslope();


	if(situation == LINEAR) {
		while (moreFlips && !convergence && iterationNum < 30)
		{
			if(triangle) TriangleDeformation();
			else MeshDeformation();
			float maxMovement = 0.0f;

			for (unsigned i = 0 ; i < vertexList.size() ; i++)
			{
				float length = (vertexList[i] - backupVertex[i]).length();

				if (length > maxMovement)
					maxMovement = length;
			}

			if (maxMovement > 0.5f) {
				convergence = 0;
			}
			else {
				convergence = 1;
			}

			vertexList = backupVertex;

			if(triangle) 
			{
				flip_count = 0;
				moreFlips = 0;
				int edge_size = edgeList.size();
				for (int i = 0 ; i < edge_size ; ++i)
				{
					int source = edgeList[i].x;
					int target = edgeList[i].y;
					vector2 vec0 = vertexList[source] - vertexList[target];
					vector2 vec1 = referenceList[source] - referenceList[target];

					if (DotProduct(vec0 , vec1) < -1.0f)
					{
						moreFlips = 1;
						flippedConstraintList.push_back(i);
						flip_count++;
					}
				}

				if(moreFlips)
					updateedgeslope();
			}
			else ;
			iterationNum++;

		}
	} else if(situation == QUADRATIC){
		QuadMeshDeformation();
		vertexList = backupVertex;
	}
	//printf("\niterationNum: %d\n",iterationNum);
	printf("coordinate : (%f, %f)\n",vertexList[rbIdx].x,vertexList[rbIdx].y);
}

void manager_image::SimilarityCoeff(const vector<vector4> &Ap, float **coeff)
{
	matrix44 ApTAp;

	for (int i = 0 ; i < 4 ; ++i)
	{
		for (int j = 0 ; j < 4 ; ++j)
		{
			ApTAp[i][j] = 0.0f;

			for (int m = 0 ; m < 8 ; ++m)
				ApTAp[i][j] += Ap[m][i] * Ap[m][j];
		}
	}

	ApTAp.invert();
	ApTAp.transpose();

	vector<vector4> mx;
	mx.clear();
	mx.resize(8);

	for (int i = 0 ; i < 8 ; ++i)
		mx[i] = ApTAp * Ap[i];

	for (int i = 0 ; i < 8 ; ++i)
	{
		for (int j = 0 ; j < 8 ; ++j)
		{
			coeff[i][j] = 0.0f;

			for (int m = 0 ; m < 4 ; ++m)
				coeff[i][j] += mx[i][m] * Ap[j][m];
		}
	}

	for (int i = 0 ; i < 8 ; ++i)
		coeff[i][i] -= 1.0f;
}

void manager_image::MeshDeformation()
{

	unsigned int xBoundaryNum = x_boundary.size();
	unsigned int yBoundaryNum = y_boundary.size();
	unsigned int faceNum = faceList.size();
	unsigned int structureListCenterNum = structureListCenter.size();

	int quad_count = 0;
	int patch_count = 0;



	for (int i = 0; i < patch_num ; i++)
	{
		if (patch[i].path_id !=-1 && patch[i].quad.size() > 0)
		{
			int quad_size = patch[i].quad.size();
			quad_count +=  quad_size;
			patch_count++;
		}
	}

	const int cols = 2 * vertexNum;
	const int rows =/* 8*faceNum +*/ 16*quad_count + /*2*faceNum + 2*structureListCenterNum +*/ 4*faceNum /*+ 2*patch_count*/ + xBoundaryNum + yBoundaryNum;


	static float *B = NULL;
	static float *x = NULL;
	static float ***coeff = NULL;

	B = new float[rows];
	x = new float[cols];

	int rowIdx = 0;
	int temp_count = 0;

	const float hardConst = 100.0f;
	const float edgeweight = 3.0f;
	const float structedgeweight  = 0.0f;
	const float orientationweight = 0.0f;

	// -------------------------------------- set system matrix -------------------------------------
	// aspect ratio

	if (!solver.IsTheSameSize(rows, cols))
	{
		solver.ResetSolver(rows,cols);
		// 		coeff = new float**[faceNum];
		// 		for (unsigned int i = 0 ; i < faceNum ; ++i)
		// 		{
		// 			coeff[i] = new float*[8];
		// 			for (int j = 0 ; j < 8 ; ++j)
		// 			{
		// 				coeff[i][j] = new float[8];
		// 			}
		// 		}
		// 
		// 		vector<vector4> Ap;
		// 
		// 		Ap.clear();
		// 		Ap.resize(8);

		rowIdx = 0;

		// 		for (unsigned int i = 0 ; i < faceNum ; ++i)
		// 		{
		// 			int vList[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};
		// 			float weight = faceflexibleList[i];
		// 
		// 			if(FaceSelect[i]) weight = 1.0;
		// 			// coeff only compute for first iteration
		// 			for (int j = 0 ; j < 4 ; ++j)
		// 			{
		// 				Ap[2 * j + 0].x = referenceList[vList[j]].x;
		// 				Ap[2 * j + 0].y = -referenceList[vList[j]].y;
		// 				Ap[2 * j + 0].z = 1.0f;
		// 				Ap[2 * j + 0].w = 0.0f;
		// 
		// 				Ap[2 * j + 1].x = referenceList[vList[j]].y;
		// 				Ap[2 * j + 1].y = referenceList[vList[j]].x;
		// 				Ap[2 * j + 1].z = 0.0f;
		// 				Ap[2 * j + 1].w = 1.0f;
		// 			}
		// 
		// 			SimilarityCoeff(Ap, coeff[i]);  
		// 
		// 
		// 			for (int j = 0 ; j < 8 ; ++j)
		// 			{
		// 				if (fabs(coeff[i][j][0]) > 0.00001f)	solver.AddSysElement(rowIdx, vList[0],				weight * coeff[i][j][0]);
		// 				if (fabs(coeff[i][j][1]) > 0.00001f)	solver.AddSysElement(rowIdx, vertexNum + vList[0],	weight * coeff[i][j][1]);
		// 				if (fabs(coeff[i][j][2]) > 0.00001f)	solver.AddSysElement(rowIdx, vList[1],				weight * coeff[i][j][2]);
		// 				if (fabs(coeff[i][j][3]) > 0.00001f)	solver.AddSysElement(rowIdx, vertexNum + vList[1],	weight * coeff[i][j][3]);
		// 				if (fabs(coeff[i][j][4]) > 0.00001f)	solver.AddSysElement(rowIdx, vList[2],				weight * coeff[i][j][4]);
		// 				if (fabs(coeff[i][j][5]) > 0.00001f)	solver.AddSysElement(rowIdx, vertexNum + vList[2],	weight * coeff[i][j][5]);
		// 				if (fabs(coeff[i][j][6]) > 0.00001f)	solver.AddSysElement(rowIdx, vList[3],				weight * coeff[i][j][6]);
		// 				if (fabs(coeff[i][j][7]) > 0.00001f)	solver.AddSysElement(rowIdx, vertexNum + vList[3],	weight * coeff[i][j][7]);
		// 				++rowIdx;
		// 			}
		// 		}

		int center_edge_index = 0;
		for(int i = 0 ; i < patch_num ; ++i)
		{
			if(patch[i].path_id != -1 && patch[i].quad.size() > 0)
			{


				int patch_quad_size = patch[i].quad.size();

				temp_count += patch_quad_size;


				int v1 = patch_center_edge[i].x;
				int v2 = patch_center_edge[i].y;

				double dx = referenceList[v1].x  - referenceList[v2].x;
				double dy = referenceList[v1].y  - referenceList[v2].y;
				double center_delta = -dx * dx - dy * dy; 
				double SR[2][2];
				SR[0][0] = -dx / (center_delta+0.00001);
				SR[0][1] = -dy / (center_delta+0.00001);
				SR[1][0] = -dy / (center_delta+0.00001);
				SR[1][1] =  dx / (center_delta+0.00001);


				// 					double T[2][2];
				// 					T[0][0] = dx*dx;
				// 					T[0][1] = dx*dy;
				// 					T[1][0] = dx*dy;
				// 					T[1][1] = dy*dy;
				// 					double delta = T[0][0]*T[1][1] - T[0][1]*T[1][0];
				// 					double T_inv[2][2];
				// 					T_inv[0][0] =  T[1][1] / (delta + 0.0001);
				// 					T_inv[0][1] = -T[0][1] / (delta + 0.0001);
				// 					T_inv[1][0] = -T[1][0] / (delta + 0.0001);
				// 					T_inv[1][1] =  T[0][0] / (delta + 0.0001);

				for (int j = 0 ; j < patch_quad_size ; j++)
				{
					int face_index = patch[i].quad[j];
					int vList[5]  = {faceList[face_index].x , faceList[face_index].y , faceList[face_index].z , faceList[face_index].w,faceList[face_index].x};

					float weight = 5.0f *  patch[i].saliency /*faceflexibleList[face_index]*/;
					if(FaceSelect[face_index]) weight = 6.0f;	

					for(int k = 0 ; k < 4 ; k++)
					{
						vector2 temp_edge;
						temp_edge.set((referenceList[vList[k]].x - referenceList[vList[k+1]].x) , (referenceList[vList[k]].y - referenceList[vList[k+1]].y));
						double _S =  (SR[0][0] * temp_edge.x + SR[0][1] * temp_edge.y);
						double _R =  (SR[1][0] * temp_edge.x + SR[1][1] * temp_edge.y);


						// 							double T[2][2];
						// 							T[0][0] = temp_edge.x*temp_edge.x;
						// 							T[0][1] = temp_edge.x*temp_edge.y;
						// 							T[1][0] = temp_edge.x*temp_edge.y;
						// 							T[1][1] = temp_edge.y*temp_edge.y;
						// 							double delta = T[0][0]*T[1][1] - T[0][1]*T[1][0];
						// 							double T_inv[2][2];
						// 							T_inv[0][0] =  T[1][1] / (delta + 0.0001);
						// 							T_inv[0][1] = -T[0][1] / (delta + 0.0001);
						// 							T_inv[1][0] = -T[1][0] / (delta + 0.0001);
						// 							T_inv[1][1] =  T[0][0] / (delta + 0.0001);

						// 							double a = (T_inv[0][0] * temp_edge.x + T_inv[0][1] * temp_edge.y)*dx;
						// 							double b = (T_inv[0][0] * temp_edge.x + T_inv[0][1] * temp_edge.y)*dy;
						// 							double c = (T_inv[1][0] * temp_edge.x + T_inv[1][1] * temp_edge.y)*dx;
						// 						    double d = (T_inv[1][0] * temp_edge.x + T_inv[1][1] * temp_edge.y)*dy;

						// 							double a = (T_inv[0][0] * dx + T_inv[0][1] * dy)*temp_edge.x;
						// 							double b = (T_inv[0][0] * dx + T_inv[0][1] * dy)*temp_edge.y;
						// 							double c = (T_inv[1][0] * dx + T_inv[1][1] * dy)*temp_edge.x;
						// 							double d = (T_inv[1][0] * dx + T_inv[1][1] * dy)*temp_edge.y; 

						solver.AddSysElement(rowIdx, vList[k]       ,	 weight );  
						solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight );
						solver.AddSysElement(rowIdx, v1             ,	-_S*weight);
						solver.AddSysElement(rowIdx, vertexNum + v1 ,	-_R*weight );
						solver.AddSysElement(rowIdx, v2             ,	 _S*weight );
						solver.AddSysElement(rowIdx, vertexNum + v2 ,	 _R*weight );

						rowIdx++;

						solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight );
						solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight );
						solver.AddSysElement(rowIdx, v1                         ,	 _R*weight );
						solver.AddSysElement(rowIdx, vertexNum + v1             ,	-_S*weight );
						solver.AddSysElement(rowIdx, v2                         ,	-_R*weight );
						solver.AddSysElement(rowIdx, vertexNum + v2             ,	 _S*weight );

						rowIdx++;
						// 							solver.AddSysElement(rowIdx, vList[k]       ,	 weight );
						// 							solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight );
						// 							solver.AddSysElement(rowIdx, v1             ,	-a*weight);
						// 							solver.AddSysElement(rowIdx, vertexNum + v1 ,	-b*weight );
						// 							solver.AddSysElement(rowIdx, v2             ,	 a*weight );
						// 							solver.AddSysElement(rowIdx, vertexNum + v2 ,	 b*weight );
						// 
						// 							rowIdx++;
						// 
						// 							solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight );
						// 							solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight );
						// 							solver.AddSysElement(rowIdx, v1                         ,	-c*weight );
						// 							solver.AddSysElement(rowIdx, vertexNum + v1             ,	-d*weight );
						// 							solver.AddSysElement(rowIdx, v2                         ,	 c*weight );
						// 							solver.AddSysElement(rowIdx, vertexNum + v2             ,	 d*weight );
						// 
						// 							rowIdx++;
					}



					float resize_ratio_x = vertexList[rbIdx].x / referenceList[rbIdx].x;
					float resize_ratio_y = vertexList[rbIdx].y / referenceList[rbIdx].y;

					float weight2 = 1.0f *  (1-patch[i].saliency)  /*(1-faceflexibleList[face_index])*/;

					double dlx = (referenceList[v1].x  - referenceList[v2].x)*resize_ratio_x;
					double dly = (referenceList[v1].y  - referenceList[v2].y)*resize_ratio_y;
					double centerl_delta = -dlx * dlx - dly * dly; 
					double lSR[2][2];
					lSR[0][0] = -dlx / (centerl_delta+0.00001);
					lSR[0][1] = -dly / (centerl_delta+0.00001);
					lSR[1][0] = -dly / (centerl_delta+0.00001);
					lSR[1][1] =  dlx / (centerl_delta+0.00001);

					// 						double TT[2][2];
					// 						TT[0][0] = dlx*dlx;
					// 						TT[0][1] = dlx*dly;
					// 						TT[1][0] = dlx*dly;
					// 						TT[1][1] = dly*dly;
					// 						double delta = TT[0][0]*TT[1][1] - TT[0][1]*TT[1][0];
					// 						double TT_inv[2][2];
					// 						TT_inv[0][0] =  TT[1][1] / (delta + 0.0001);
					// 						TT_inv[0][1] = -TT[0][1] / (delta + 0.0001);
					// 						TT_inv[1][0] = -TT[1][0] / (delta + 0.0001);
					// 						TT_inv[1][1] =  TT[0][0] / (delta + 0.0001);

					for(int k = 0 ; k < 4 ; k++)
					{
						vector2 ltemp_edge;
						ltemp_edge.set(resize_ratio_x*(referenceList[vList[k]].x - referenceList[vList[k+1]].x) , resize_ratio_y*(referenceList[vList[k]].y - referenceList[vList[k+1]].y));
						double __S =  (lSR[0][0] * ltemp_edge.x + lSR[0][1] * ltemp_edge.y);
						double __R =  (lSR[1][0] * ltemp_edge.x + lSR[1][1] * ltemp_edge.y);

						// 							double TT[2][2];
						// 							TT[0][0] = ltemp_edge.x*ltemp_edge.x;
						// 							TT[0][1] = ltemp_edge.x*ltemp_edge.y;
						// 							TT[1][0] = ltemp_edge.x*ltemp_edge.y;
						// 							TT[1][1] = ltemp_edge.y*ltemp_edge.y;
						// 							double delta = TT[0][0]*TT[1][1] - TT[0][1]*TT[1][0];
						// 							double TT_inv[2][2];
						// 							TT_inv[0][0] =  TT[1][1] / (delta + 0.0001);
						// 							TT_inv[0][1] = -TT[0][1] / (delta + 0.0001);
						// 							TT_inv[1][0] = -TT[1][0] / (delta + 0.0001);
						// 							TT_inv[1][1] =  TT[0][0] / (delta + 0.0001);

						// 							double aa = (TT_inv[0][0] * dlx + TT_inv[0][1] * dly)*ltemp_edge.x;
						// 							double bb = (TT_inv[0][0] * dlx + TT_inv[0][1] * dly)*ltemp_edge.y;
						// 							double cc = (TT_inv[1][0] * dlx + TT_inv[1][1] * dly)*ltemp_edge.x;
						// 							double dd = (TT_inv[1][0] * dlx + TT_inv[1][1] * dly)*ltemp_edge.y; 

						solver.AddSysElement(rowIdx, vList[k]       ,	 weight2 );
						solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight2 );
						solver.AddSysElement(rowIdx, v1             ,	-__S*weight2);
						solver.AddSysElement(rowIdx, vertexNum + v1 ,	-__R*weight2 );
						solver.AddSysElement(rowIdx, v2             ,	 __S*weight2 );
						solver.AddSysElement(rowIdx, vertexNum + v2 ,	 __R*weight2 );

						rowIdx++;

						solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight2 );
						solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight2 );
						solver.AddSysElement(rowIdx, v1                         ,	 __R*weight2 );
						solver.AddSysElement(rowIdx, vertexNum + v1             ,	-__S*weight2 );
						solver.AddSysElement(rowIdx, v2                         ,	-__R*weight2 );
						solver.AddSysElement(rowIdx, vertexNum + v2             ,	 __S*weight2 );

						rowIdx++;
						// 							solver.AddSysElement(rowIdx, vList[k]       ,	 weight2 );
						// 							solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight2 );
						// 							solver.AddSysElement(rowIdx, v1             ,	-aa*weight2);
						// 							solver.AddSysElement(rowIdx, vertexNum + v1 ,	-bb*weight2 );
						// 							solver.AddSysElement(rowIdx, v2             ,	 aa*weight2 );
						// 							solver.AddSysElement(rowIdx, vertexNum + v2 ,	 bb*weight2 );
						// 
						// 							rowIdx++;
						// 
						// 							solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight2 );
						// 							solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight2 );
						// 							solver.AddSysElement(rowIdx, v1             ,				-cc*weight2);
						// 							solver.AddSysElement(rowIdx, vertexNum + v1 ,				-dd*weight2 );
						// 							solver.AddSysElement(rowIdx, v2             ,				 cc*weight2 );
						// 							solver.AddSysElement(rowIdx, vertexNum + v2 ,				 dd*weight2 );
						// 							rowIdx++;
					}


				}
				center_edge_index++;



				// 					for(int k = 0 ; k < 3 ; k++)
				// 					{
				// 						float m = resize_ratio_y*(referenceList[vList[k+1]].y - referenceList[vList[k]].y) / (resize_ratio_x*(referenceList[vList[k+1]].x - referenceList[vList[k]].x));
				// 						solver.AddSysElement(rowIdx  , vertexNum + vList[k+1]   ,	 weight2 );
				// 						solver.AddSysElement(rowIdx++, vertexNum + vList[k]     ,	-weight2 );
				// 						solver.AddSysElement(rowIdx  , vList[k+1]               ,	 weight2 );
				// 						solver.AddSysElement(rowIdx++, vList[k]                 ,	-weight2 );
				// 
				// 					}

				// 				}
				// 				else
				// 				{
				// 					int v1 = center_edge_back.x;
				// 					int v2 = center_edge_back.y;
				// 					double dx = referenceList[v1].x  - referenceList[v2].x;
				// 					double dy = referenceList[v1].y  - referenceList[v2].y;
				// 					double center_delta = -dx * dx - dy * dy; 
				// 					double SR[2][2];
				// 					SR[0][0] = -dx / (center_delta+0.00001);
				// 					SR[0][1] = -dy / (center_delta+0.00001);
				// 					SR[1][0] = -dy / (center_delta+0.00001);
				// 					SR[1][1] =  dx / (center_delta+0.00001);
				// 
				// 					int vList[4] = {faceList[i].x , faceList[i].y , faceList[i].z , faceList[i].x};
				// 
				// 					float weight = 1.0 * nonimportantpatch;
				// 
				// 
				// 					for(int k = 0 ; k < 3 ; k++)
				// 					{
				// 						vector2 temp_edge;
				// 						temp_edge.set((referenceList[vList[k]].x - referenceList[vList[k+1]].x) , (referenceList[vList[k]].y - referenceList[vList[k+1]].y));
				// 						double _S =  SR[0][0] * temp_edge.x + SR[0][1] * temp_edge.y;
				// 						double _R =  SR[1][0] * temp_edge.x + SR[1][1] * temp_edge.y;
				// 
				// 						solver.AddSysElement(rowIdx, vList[k]       ,	 weight );
				// 						solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight );
				// 						solver.AddSysElement(rowIdx, v1             ,	-_S*weight);
				// 						solver.AddSysElement(rowIdx, vertexNum + v1 ,	-_R*weight );
				// 						solver.AddSysElement(rowIdx, v2             ,	 _S*weight );
				// 						solver.AddSysElement(rowIdx, vertexNum + v2 ,	 _R*weight );
				// 
				// 						rowIdx++;
				// 
				// 						solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight );
				// 						solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight );
				// 						solver.AddSysElement(rowIdx, v1                         ,	 _R*weight );
				// 						solver.AddSysElement(rowIdx, vertexNum + v1             ,	-_S*weight );
				// 						solver.AddSysElement(rowIdx, v2                         ,	-_R*weight );
				// 						solver.AddSysElement(rowIdx, vertexNum + v2             ,	 _S*weight );
				// 
				// 						rowIdx++;
				// 					}
				// 
				// 					float resize_ratio_x = vertexList[rbIdx].x / referenceList[rbIdx].x;
				// 					float resize_ratio_y = vertexList[rbIdx].y / referenceList[rbIdx].y;
				// 
				// 					float weight2 = 5.0 * nonimportantpatch;
				// 
				// 					dx = (referenceList[v1].x  - referenceList[v2].x)*resize_ratio_x;
				// 					dy = (referenceList[v1].y  - referenceList[v2].y)*resize_ratio_y;
				// 					center_delta = -dx * dx - dy * dy; 
				// 
				// 					SR[0][0] = -dx / (center_delta+0.00001);
				// 					SR[0][1] = -dy / (center_delta+0.00001);
				// 					SR[1][0] = -dy / (center_delta+0.00001);
				// 					SR[1][1] =  dx / (center_delta+0.00001);
				// 
				// 					for(int k = 0 ; k < 3 ; k++)
				// 					{
				// 						vector2 temp_edge;
				// 						temp_edge.set(resize_ratio_x*(referenceList[vList[k]].x - referenceList[vList[k+1]].x) , resize_ratio_y*(referenceList[vList[k]].y - referenceList[vList[k+1]].y));
				// 						double _S =  SR[0][0] * temp_edge.x + SR[0][1] * temp_edge.y;
				// 						double _R =  SR[1][0] * temp_edge.x + SR[1][1] * temp_edge.y;
				// 
				// 						solver.AddSysElement(rowIdx, vList[k]       ,	 weight2 );
				// 						solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight2 );
				// 						solver.AddSysElement(rowIdx, v1             ,	-_S*weight2);
				// 						solver.AddSysElement(rowIdx, vertexNum + v1 ,	-_R*weight2 );
				// 						solver.AddSysElement(rowIdx, v2             ,	 _S*weight2 );
				// 						solver.AddSysElement(rowIdx, vertexNum + v2 ,	 _R*weight2 );
				// 
				// 						rowIdx++;
				// 
				// 						solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight2 );
				// 						solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight2 );
				// 						solver.AddSysElement(rowIdx, v1                         ,	 _R*weight2 );
				// 						solver.AddSysElement(rowIdx, vertexNum + v1             ,	-_S*weight2 );
				// 						solver.AddSysElement(rowIdx, v2                         ,	-_R*weight2 );
				// 						solver.AddSysElement(rowIdx, vertexNum + v2             ,	 _S*weight2 );
				// 
				// 						rowIdx++;
				// 					}
				// 					for(int k = 0 ; k < 3 ; k++)
				// 					{
				// 						float m = resize_ratio_y*(referenceList[vList[k+1]].y - referenceList[vList[k]].y) / (resize_ratio_x*(referenceList[vList[k+1]].x - referenceList[vList[k]].x));
				// 						solver.AddSysElement(rowIdx  , vertexNum + vList[k+1]   ,	 weight2 );
				// 						solver.AddSysElement(rowIdx++, vertexNum + vList[k]     ,	-weight2 );
				// 						solver.AddSysElement(rowIdx  , vList[k+1]               ,	 weight2 );
				// 						solver.AddSysElement(rowIdx++, vList[k]                 ,	-weight2 );
				// 
				// 					}

			}


		}

		for(unsigned int i = 0 ; i < faceNum ; ++i)
		{
			int vList[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};
			solver.AddSysElement(rowIdx  , vList[0],     edgeweight);
			solver.AddSysElement(rowIdx++, vList[3],	-edgeweight);
			solver.AddSysElement(rowIdx  , vList[1],     edgeweight);
			solver.AddSysElement(rowIdx++, vList[2],	-edgeweight);
			solver.AddSysElement(rowIdx  , vertexNum + vList[0],     edgeweight);
			solver.AddSysElement(rowIdx++, vertexNum + vList[1],	-edgeweight);
			solver.AddSysElement(rowIdx  , vertexNum + vList[3],     edgeweight);
			solver.AddSysElement(rowIdx++, vertexNum + vList[2],	-edgeweight);
		}

		// 		for(unsigned int i = 0 ; i < structureListCenterNum ; ++i)
		// 		{
		// 			int vList[4] = {structureList[i].x, structureList[i].y, structureList[i].z, structureList[i].w};
		// 			int vList2[4] = {structureList[i].x, structureList[i].y, structureList[i].z, structureList[i].w};
		// 			int CentervList = structureListCenter[i];
		// 			               
		// 			solver.AddSysElement(rowIdx    , CentervList,     8*structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vList[0],        -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vList[1],        -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vList[2],        -structedgeweight);
		// 			solver.AddSysElement(rowIdx	   , vList[3],        -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vList2[0],       -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vList2[1],       -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vList2[2],       -structedgeweight);
		// 			solver.AddSysElement(rowIdx++  , vList2[3],       -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vertexNum + CentervList,    8*structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vertexNum + vList[0],        -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vertexNum + vList[1],        -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vertexNum + vList[2],        -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vertexNum + vList[3],        -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vertexNum + vList2[0],       -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vertexNum + vList2[1],       -structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vertexNum + vList2[2],       -structedgeweight);
		// 			solver.AddSysElement(rowIdx++  , vertexNum + vList2[3],       -structedgeweight);
		// 	
		// 		}

		// 		for(unsigned int i = 0 ; i < faceNum ; ++i)
		// 		{                                                         
		// 			int vList[4] = {faceList[i].x, faceList[i].y, faceList[i].z, faceList[i].w};
		//    
		// 			solver.AddSysElement(rowIdx    ,        i,           structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vList[0],        -structedgeweight/4);
		// 			solver.AddSysElement(rowIdx    , vList[1],        -structedgeweight/4);
		// 			solver.AddSysElement(rowIdx    , vList[2],        -structedgeweight/4);
		// 			solver.AddSysElement(rowIdx++  , vList[3],        -structedgeweight/4);
		// 
		// 			solver.AddSysElement(rowIdx    , vertexNum +        i,           structedgeweight);
		// 			solver.AddSysElement(rowIdx    , vertexNum + vList[0],        -structedgeweight/4);
		// 			solver.AddSysElement(rowIdx    , vertexNum + vList[1],        -structedgeweight/4);
		// 			solver.AddSysElement(rowIdx    , vertexNum + vList[2],        -structedgeweight/4);
		// 			solver.AddSysElement(rowIdx++  , vertexNum + vList[3],        -structedgeweight/4);
		// 		}

		// 		for (unsigned int i = 0 ; i < patch_num ; ++i)
		// 		{
		// 			if(patch[i].path_id != -1 && patch[i].quad.size() > 0)
		// 			{
		// 				int v1 = patch_center_edge[i].x;
		// 				int v2 = patch_center_edge[i].y;
		// 				float weight = 1.0f;
		// 				
		// 				double fixedge_x = referenceList[x_boundary[0]].x  - referenceList[x_boundary[xBoundaryNum-1]].x;
		// 				double fixedge_y = referenceList[x_boundary[0]].y  - referenceList[x_boundary[xBoundaryNum-1]].y;
		// 
		// 				double dx = referenceList[v1].x  - referenceList[v2].x;
		// 				double dy = referenceList[v1].y  - referenceList[v2].y;
		// 				double costhta = (fixedge_x * dx + fixedge_y * dy) / (sqrt(fixedge_x*fixedge_x + fixedge_y*fixedge_y) + sqrt(dx*dx+dy*dy)); 
		// 				int vList1[2] = {v1, v2};
		// 				int vList[2] = {x_boundary[0], x_boundary[xBoundaryNum-1]};
		//    				solver.AddSysElement(rowIdx,	   vList1[0],				  orientationweight*costhta*weight );  
		// 				solver.AddSysElement(rowIdx,	   vList1[1],				 -orientationweight*costhta*weight );
		//                 solver.AddSysElement(rowIdx,       vList[0],        		         -orientationweight*weight );
		// 				solver.AddSysElement(rowIdx++,     vList[1],        		          orientationweight*weight );
		// 				solver.AddSysElement(rowIdx,     vertexNum + vList1[0],	      orientationweight*costhta*weight );  
		// 				solver.AddSysElement(rowIdx,     vertexNum + vList1[1],	     -orientationweight*costhta*weight );
		// 				solver.AddSysElement(rowIdx,     vertexNum + vList[0] ,	             -orientationweight*weight );
		// 				solver.AddSysElement(rowIdx++,   vertexNum + vList[1] ,               orientationweight*weight );			
		// 			}
		// 		}



		for (unsigned int i = 0 ; i < xBoundaryNum ; ++i) {
			solver.AddSysElement(rowIdx++ , x_boundary[i] , hardConst);
		}
		for (unsigned int i = 0 ; i < yBoundaryNum ; ++i) {
			solver.AddSysElement(rowIdx++ , vertexNum + y_boundary[i] , hardConst);
		}	 
	}


	// ------------------------------------ set right hand size matrix --------------------------------
	rowIdx = 0;

	// 	for (unsigned int i = 0 ; i < 8*faceNum ; ++i)
	// 	{
	// 		B[rowIdx++] = 0.0f;
	// 	}
	for(int i = 0 ; i < 16 * quad_count ; ++i)
	{
		B[rowIdx++] = 0.0f;
	}

	for (unsigned int i = 0 ; i < 4*faceNum ; ++i)
	{
		B[rowIdx++] = 0.0f;
	}

	// 	for (unsigned int i = 0 ; i < 2*structureListCenterNum ; ++i)
	// 	{
	// 		B[rowIdx++] = 0.0f;
	// 	}
	// 	for (unsigned int i = 0 ; i < 2*faceNum ; ++i)
	// 	{
	// 		B[rowIdx++] = 0.0f;
	// 	}

	// 	for (unsigned int i = 0 ; i < 2*patch_count ; ++i)
	// 	{
	// 			B[rowIdx++] = 0.0f;
	// 
	// 	}


	for (unsigned int i = 0 ; i < xBoundaryNum ; ++i)	// x
	{
		if(referenceList[x_boundary[i]].x == referenceList[0].x)
			B[rowIdx++] = hardConst * 0.0f;
		else
			B[rowIdx++] = hardConst * vertexList[rbIdx].x;
	}
	for (unsigned int i = 0 ; i < yBoundaryNum ; ++i)	// y
	{
		if(referenceList[y_boundary[i]].y == referenceList[0].y)
			B[rowIdx++] = hardConst * 0.0f;
		else
			B[rowIdx++] = hardConst * vertexList[rbIdx].y;
	}


	// Set right hand size matrix
	solver.SetRightHandSideMatrix(B);

	// ------------------------------------------ initial guess ---------------------------------------
	rowIdx = 0;

	for (int i = 0 ; i < vertexNum ; i++)
	{
		x[rowIdx] =  vertexList[i].x;
		x[vertexNum + rowIdx] = vertexList[i].y;
		rowIdx++;
	}

	solver.SetInitialGuess(x);

	// ---------------------------------------------- solve -------------------------------------------
	solver.solve();

	backupVertex.clear();
	rowIdx = 0;
	for(int i = 0 ; i < vertexNum ; i++)
	{
		vector2 pos(solver.GetSolution(rowIdx) , solver.GetSolution(vertexNum + rowIdx));

		//printf("(%f %f) => (%f %f)\n",referenceList[i].x,referenceList[i].y,temp.x,temp.y);
		backupVertex.push_back(pos);

		rowIdx++;
	}

}

void manager_image::TriangleSimilarityCoeff(const vector<vector4> &Ap, float **coeff)
{
	matrix44 ApTAp;

	for (int i = 0 ; i < 4 ; ++i)
	{
		for (int j = 0 ; j < 4 ; ++j)
		{
			ApTAp[i][j] = 0.0f;

			for (int m = 0 ; m < 6 ; ++m)
				ApTAp[i][j] += Ap[m][i] * Ap[m][j];
		}
	}

	ApTAp.invert();
	ApTAp.transpose();

	vector<vector4> mx;
	mx.clear();
	mx.resize(6);

	for (int i = 0 ; i < 6 ; ++i)
		mx[i] = ApTAp * Ap[i];

	for (int i = 0 ; i < 6 ; ++i)
	{
		for (int j = 0 ; j < 6 ; ++j)
		{
			coeff[i][j] = 0.0f;

			for (int m = 0 ; m < 4 ; ++m)
				coeff[i][j] += mx[i][m] * Ap[j][m];
		}
	}

	for (int i = 0 ; i < 6 ; ++i)
		coeff[i][i] -= 1.0f;
}

void manager_image::TriangleDeformation()
{
	unsigned int xBoundaryNum = x_boundary.size();
	unsigned int yBoundaryNum = y_boundary.size();
	unsigned int faceNum = faceList.size();
	unsigned int flippedConstraintNum = flippedConstraintList.size();
	unsigned int structure_vertexNum = 0;
	for(int i = 0 ; i < vertex_table.size() ; ++i)
	{
		if(vertex_table[i].size() >= 3) structure_vertexNum++;
	}

	int triangle_count = 0;
	int patch_count = 0;

	for (int i = 0; i < patch_num ; i++)
	{
		if (patch[i].path_id !=-1 && patch[i].triangle.size() > 0)
		{
			int triangle_size = patch[i].triangle.size();
			triangle_count +=  triangle_size;
			patch_count++;
		}
	}


	// 	if(Mask_Image)
	// 	{
	// 		for(int i = 0 ; i < faceList.size() ; ++i)
	// 		{
	// 			if(cluster_index[i] == 1) f_triangle_count++;
	// 			else                      b_triangle_count++;
	// 		}
	// 	}

	int edge_count = 0;

	for (unsigned int i = 0 ; i < detectedge.size() ; ++i)
	{
		for(unsigned int j = 0 ; j < detectedge[i].size() ; ++j)
		{
			edge_count++;
		}
	}



	const int cols = 2 * vertexNum;
	const int rows =/* 6*faceNum*/ + xBoundaryNum + yBoundaryNum + 12*triangle_count + 2 * flippedConstraintNum +  edge_count + 2 * structure_vertexNum + 2*patch_count;

	static float *B = NULL;
	static float *x = NULL;
	static float ***coeff = NULL;

	B = new float[rows];
	x = new float[cols];

	int rowIdx = 0;

	const float hardConst = 100.0f;
	float structedgeweight = 0.0f;
	const float orientationweight = 0.0f;

	int temp_count = 0;
	//	const float edgeweight = 100.0f;

	// -------------------------------------- set system matrix -------------------------------------
	// aspect ratio

	if (!solver.IsTheSameSize(rows, cols))
	{
		solver.ResetSolver(rows,cols);
		// 		coeff = new float**[faceNum];
		// 		for (unsigned int i = 0 ; i < faceNum ; ++i)
		// 		{
		// 			coeff[i] = new float*[6];
		// 			for (int j = 0 ; j < 6 ; ++j)
		// 			{
		// 				coeff[i][j] = new float[6];
		// 			}
		// 		}
		//
		// 		vector<vector4> Ap;
		// 
		// 		Ap.clear();
		// 		Ap.resize(6);
		// 
		// 		rowIdx = 0;
		//
		// 		for (unsigned int i = 0 ; i < faceNum ; ++i)
		// 		{
		// 			int vList[3] = {faceList[i].x,faceList[i].y,faceList[i].z};
		// 			float weight = faceflexibleList[i];
		// 
		// 			if(FaceSelect[i]) weight = 1.0;
		// 			// coeff only compute for first iteration
		// 			for (int j = 0 ; j < 3 ; ++j)
		// 			{
		// 				Ap[2 * j + 0].x = referenceList[vList[j]].x;
		// 				Ap[2 * j + 0].y = -referenceList[vList[j]].y;
		// 				Ap[2 * j + 0].z = 1.0f;
		// 				Ap[2 * j + 0].w = 0.0f;
		// 
		// 				Ap[2 * j + 1].x = referenceList[vList[j]].y;
		// 				Ap[2 * j + 1].y = referenceList[vList[j]].x;
		// 				Ap[2 * j + 1].z = 0.0f;
		// 				Ap[2 * j + 1].w = 1.0f;
		// 			}
		// 
		// 			TriangleSimilarityCoeff(Ap, coeff[i]);  
		// 
		// 
		// 			for (int j = 0 ; j < 6 ; ++j)
		// 			{
		// 				if (fabs(coeff[i][j][0]) > 0.00001f)	solver.AddSysElement(rowIdx, vList[0],				weight * coeff[i][j][0]);
		// 				if (fabs(coeff[i][j][1]) > 0.00001f)	solver.AddSysElement(rowIdx, vertexNum + vList[0],	weight * coeff[i][j][1]);
		// 				if (fabs(coeff[i][j][2]) > 0.00001f)	solver.AddSysElement(rowIdx, vList[1],				weight * coeff[i][j][2]);
		// 				if (fabs(coeff[i][j][3]) > 0.00001f)	solver.AddSysElement(rowIdx, vertexNum + vList[1],	weight * coeff[i][j][3]);
		// 				if (fabs(coeff[i][j][4]) > 0.00001f)	solver.AddSysElement(rowIdx, vList[2],				weight * coeff[i][j][4]);
		// 				if (fabs(coeff[i][j][5]) > 0.00001f)	solver.AddSysElement(rowIdx, vertexNum + vList[2],	weight * coeff[i][j][5]);
		// 				++rowIdx;
		// 			}
		// 		}

		/*
		if(Mask_Image)
		{
		for(int i = 0 ; i <faceList.size() ; ++i)
		{
		if(cluster_index[i] == 1)
		{

		int v1 = center_edge.x;
		int v2 = center_edge.y;
		double dx = referenceList[v1].x  - referenceList[v2].x;
		double dy = referenceList[v1].y  - referenceList[v2].y;
		double center_delta = -dx * dx - dy * dy; 
		double SR[2][2];
		SR[0][0] = -dx / (center_delta+0.00001);
		SR[0][1] = -dy / (center_delta+0.00001);
		SR[1][0] = -dy / (center_delta+0.00001);
		SR[1][1] =  dx / (center_delta+0.00001);

		int vList[4] = {faceList[i].x , faceList[i].y , faceList[i].z , faceList[i].x};

		float weight = 1.0 * importantpatch;


		for(int k = 0 ; k < 3 ; k++)
		{
		vector2 temp_edge;
		temp_edge.set((referenceList[vList[k]].x - referenceList[vList[k+1]].x) , (referenceList[vList[k]].y - referenceList[vList[k+1]].y));
		double _S =  SR[0][0] * temp_edge.x + SR[0][1] * temp_edge.y;
		double _R =  SR[1][0] * temp_edge.x + SR[1][1] * temp_edge.y;

		solver.AddSysElement(rowIdx, vList[k]       ,	 weight );
		solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight );
		solver.AddSysElement(rowIdx, v1             ,	-_S*weight);
		solver.AddSysElement(rowIdx, vertexNum + v1 ,	-_R*weight );
		solver.AddSysElement(rowIdx, v2             ,	 _S*weight );
		solver.AddSysElement(rowIdx, vertexNum + v2 ,	 _R*weight );

		rowIdx++;

		solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight );
		solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight );
		solver.AddSysElement(rowIdx, v1                         ,	 _R*weight );
		solver.AddSysElement(rowIdx, vertexNum + v1             ,	-_S*weight );
		solver.AddSysElement(rowIdx, v2                         ,	-_R*weight );
		solver.AddSysElement(rowIdx, vertexNum + v2             ,	 _S*weight );

		rowIdx++;
		}

		}
		else
		{
		int vList[4] = {faceList[i].x , faceList[i].y , faceList[i].z , faceList[i].x};

		float resize_ratio_x = vertexList[rbIdx].x / referenceList[rbIdx].x;
		float resize_ratio_y = vertexList[rbIdx].y / referenceList[rbIdx].y;

		float weight = 1.0 * nonimportantpatch;

		for(int k = 0 ; k < 3 ; k++)
		{
		float m = resize_ratio_y*(referenceList[vList[k+1]].y - referenceList[vList[k]].y) / (resize_ratio_x*(referenceList[vList[k+1]].x - referenceList[vList[k]].x));
		solver.AddSysElement(rowIdx  , vertexNum + vList[k+1]   ,	 weight );
		solver.AddSysElement(rowIdx++, vertexNum + vList[k]     ,	-weight );
		solver.AddSysElement(rowIdx  , vList[k+1]               ,	 weight );
		solver.AddSysElement(rowIdx++, vList[k]                 ,	-weight );

		}

		}
		}

		}
		*/


		//		if(Mask_Image)
		//		{
		int center_edge_index = 0;
		for(int i = 0 ; i < patch_num ; ++i)
		{
			if(patch[i].path_id != -1 && patch[i].triangle.size() > 0)
			{

				int patch_triangle_size = patch[i].triangle.size();

				temp_count += patch_triangle_size;


				int v1 = patch_center_edge[i].x;
				int v2 = patch_center_edge[i].y;

				double dx = referenceList[v1].x  - referenceList[v2].x;
				double dy = referenceList[v1].y  - referenceList[v2].y;
				double center_delta = -dx * dx - dy * dy; 
				double SR[2][2];
				SR[0][0] = -dx / (center_delta+0.00001);
				SR[0][1] = -dy / (center_delta+0.00001);
				SR[1][0] = -dy / (center_delta+0.00001);
				SR[1][1] =  dx / (center_delta+0.00001);

				// 					double T[2][2];
				// 					T[0][0] = dx*dx;
				// 					T[0][1] = dx*dy;
				// 					T[1][0] = dx*dy;
				// 					T[1][1] = dy*dy;
				// 					double delta = T[0][0]*T[1][1] - T[0][1]*T[1][0];
				// 					double T_inv[2][2];
				// 					T_inv[0][0] =  T[1][1] / (delta + 0.0001);
				// 					T_inv[0][1] = -T[0][1] / (delta + 0.0001);
				// 					T_inv[1][0] = -T[1][0] / (delta + 0.0001);
				// 					T_inv[1][1] =  T[0][0] / (delta + 0.0001);



				for (int j = 0 ; j < patch_triangle_size ; j++)
				{
					int face_index = patch[i].triangle[j];
					int vList[4]  = {faceList[face_index].x , faceList[face_index].y , faceList[face_index].z , faceList[face_index].x};

					float weight =  0.8 * patch[i].saliency /*faceflexibleList[face_index]*/;
					if(FaceSelect[face_index]) weight = 1.0;	

					for(int k = 0 ; k < 3 ; k++)
					{
						vector2 temp_edge;
						temp_edge.set((referenceList[vList[k]].x - referenceList[vList[k+1]].x) , (referenceList[vList[k]].y - referenceList[vList[k+1]].y));
						double _S =  SR[0][0] * temp_edge.x + SR[0][1] * temp_edge.y;
						double _R =  SR[1][0] * temp_edge.x + SR[1][1] * temp_edge.y;

						// 							double T[2][2];
						// 							T[0][0] = temp_edge.x*temp_edge.x;
						// 							T[0][1] = temp_edge.x*temp_edge.y;
						// 							T[1][0] = temp_edge.x*temp_edge.y;
						// 							T[1][1] = temp_edge.y*temp_edge.y;
						// 							double delta = T[0][0]*T[1][1] - T[0][1]*T[1][0];
						// 							double T_inv[2][2];
						// 							T_inv[0][0] =  T[1][1] / (delta + 0.0001);
						// 							T_inv[0][1] = -T[0][1] / (delta + 0.0001);
						// 							T_inv[1][0] = -T[1][0] / (delta + 0.0001);
						// 							T_inv[1][1] =  T[0][0] / (delta + 0.0001);

						// 							double a = (T_inv[0][0] * temp_edge.x + T_inv[0][1] * temp_edge.y)*dx;
						// 							double b = (T_inv[0][0] * temp_edge.x + T_inv[0][1] * temp_edge.y)*dy;
						// 							double c = (T_inv[1][0] * temp_edge.x + T_inv[1][1] * temp_edge.y)*dx;
						// 						    double d = (T_inv[1][0] * temp_edge.x + T_inv[1][1] * temp_edge.y)*dy;

						// 							double a = (T_inv[0][0] * dx + T_inv[0][1] * dy)*temp_edge.x;
						// 							double b = (T_inv[0][0] * dx + T_inv[0][1] * dy)*temp_edge.y;
						// 							double c = (T_inv[1][0] * dx + T_inv[1][1] * dy)*temp_edge.x;
						// 							double d = (T_inv[1][0] * dx + T_inv[1][1] * dy)*temp_edge.y; 

						solver.AddSysElement(rowIdx, vList[k]       ,	 weight );
						solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight );
						solver.AddSysElement(rowIdx, v1             ,	-_S*weight);
						solver.AddSysElement(rowIdx, vertexNum + v1 ,	-_R*weight );
						solver.AddSysElement(rowIdx, v2             ,	 _S*weight );
						solver.AddSysElement(rowIdx, vertexNum + v2 ,	 _R*weight );

						rowIdx++;

						solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight );
						solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight );
						solver.AddSysElement(rowIdx, v1                         ,	 _R*weight );
						solver.AddSysElement(rowIdx, vertexNum + v1             ,	-_S*weight );
						solver.AddSysElement(rowIdx, v2                         ,	-_R*weight );
						solver.AddSysElement(rowIdx, vertexNum + v2             ,	 _S*weight );

						rowIdx++;
						// 							solver.AddSysElement(rowIdx, vList[k]       ,	 weight );
						// 							solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight );
						// 							solver.AddSysElement(rowIdx, v1             ,	-a*weight);
						// 							solver.AddSysElement(rowIdx, vertexNum + v1 ,	-b*weight );
						// 							solver.AddSysElement(rowIdx, v2             ,	 a*weight );
						// 							solver.AddSysElement(rowIdx, vertexNum + v2 ,	 b*weight );
						// 
						// 							rowIdx++;
						// 
						// 							solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight );
						// 							solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight );
						// 							solver.AddSysElement(rowIdx, v1                         ,	-c*weight );
						// 							solver.AddSysElement(rowIdx, vertexNum + v1             ,	-d*weight );
						// 							solver.AddSysElement(rowIdx, v2                         ,	 c*weight );
						// 							solver.AddSysElement(rowIdx, vertexNum + v2             ,	 d*weight );
						// 
						// 							rowIdx++;
					}



					float resize_ratio_x = vertexList[rbIdx].x / referenceList[rbIdx].x;
					float resize_ratio_y = vertexList[rbIdx].y / referenceList[rbIdx].y;

					float weight2 = 0.1 * (1-patch[i].saliency) /*(1-faceflexibleList[face_index])*/;


					double dlx = (referenceList[v1].x  - referenceList[v2].x)*resize_ratio_x;
					double dly = (referenceList[v1].y  - referenceList[v2].y)*resize_ratio_y;
					double centerl_delta = -dlx * dlx - dly * dly; 
					double lSR[2][2];
					lSR[0][0] = -dlx / (centerl_delta+0.00001);
					lSR[0][1] = -dly / (centerl_delta+0.00001);
					lSR[1][0] = -dly / (centerl_delta+0.00001);
					lSR[1][1] =  dlx / (centerl_delta+0.00001);

					// 						double TT[2][2];
					// 						TT[0][0] = dlx*dlx;
					// 						TT[0][1] = dlx*dly;
					// 						TT[1][0] = dlx*dly;
					// 						TT[1][1] = dly*dly;
					// 						double delta = TT[0][0]*TT[1][1] - TT[0][1]*TT[1][0];
					// 						double TT_inv[2][2];
					// 						TT_inv[0][0] =  TT[1][1] / (delta + 0.0001);
					// 						TT_inv[0][1] = -TT[0][1] / (delta + 0.0001);
					// 						TT_inv[1][0] = -TT[1][0] / (delta + 0.0001);
					// 						TT_inv[1][1] =  TT[0][0] / (delta + 0.0001);

					for(int k = 0 ; k < 3 ; k++)
					{
						vector2 ltemp_edge;
						ltemp_edge.set(resize_ratio_x*(referenceList[vList[k]].x - referenceList[vList[k+1]].x) , resize_ratio_y*(referenceList[vList[k]].y - referenceList[vList[k+1]].y));
						double __S =  lSR[0][0] * ltemp_edge.x + lSR[0][1] * ltemp_edge.y;
						double __R =  lSR[1][0] * ltemp_edge.x + lSR[1][1] * ltemp_edge.y;

						// 							double TT[2][2];
						// 							TT[0][0] = ltemp_edge.x*ltemp_edge.x;
						// 							TT[0][1] = ltemp_edge.x*ltemp_edge.y;
						// 							TT[1][0] = ltemp_edge.x*ltemp_edge.y;
						// 							TT[1][1] = ltemp_edge.y*ltemp_edge.y;
						// 							double delta = TT[0][0]*TT[1][1] - TT[0][1]*TT[1][0];
						// 							double TT_inv[2][2];
						// 							TT_inv[0][0] =  TT[1][1] / (delta + 0.0001);
						// 							TT_inv[0][1] = -TT[0][1] / (delta + 0.0001);
						// 							TT_inv[1][0] = -TT[1][0] / (delta + 0.0001);
						// 							TT_inv[1][1] =  TT[0][0] / (delta + 0.0001);

						// 							double aa = (TT_inv[0][0] * dlx + TT_inv[0][1] * dly)*ltemp_edge.x;
						// 							double bb = (TT_inv[0][0] * dlx + TT_inv[0][1] * dly)*ltemp_edge.y;
						// 							double cc = (TT_inv[1][0] * dlx + TT_inv[1][1] * dly)*ltemp_edge.x;
						// 							double dd = (TT_inv[1][0] * dlx + TT_inv[1][1] * dly)*ltemp_edge.y; 

						solver.AddSysElement(rowIdx, vList[k]       ,	 weight2 );
						solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight2 );
						solver.AddSysElement(rowIdx, v1             ,	-__S*weight2);
						solver.AddSysElement(rowIdx, vertexNum + v1 ,	-__R*weight2 );
						solver.AddSysElement(rowIdx, v2             ,	 __S*weight2 );
						solver.AddSysElement(rowIdx, vertexNum + v2 ,	 __R*weight2 );

						rowIdx++;

						solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight2 );
						solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight2 );
						solver.AddSysElement(rowIdx, v1                         ,	 __R*weight2 );
						solver.AddSysElement(rowIdx, vertexNum + v1             ,	-__S*weight2 );
						solver.AddSysElement(rowIdx, v2                         ,	-__R*weight2 );
						solver.AddSysElement(rowIdx, vertexNum + v2             ,	 __S*weight2 );

						rowIdx++;
						// 							solver.AddSysElement(rowIdx, vList[k]       ,	 weight2 );
						// 							solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight2 );
						// 							solver.AddSysElement(rowIdx, v1             ,	-aa*weight2);
						// 							solver.AddSysElement(rowIdx, vertexNum + v1 ,	-bb*weight2 );
						// 							solver.AddSysElement(rowIdx, v2             ,	 aa*weight2 );
						// 							solver.AddSysElement(rowIdx, vertexNum + v2 ,	 bb*weight2 );
						// 
						// 							rowIdx++;
						// 
						// 							solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight2 );
						// 							solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight2 );
						// 							solver.AddSysElement(rowIdx, v1             ,				-cc*weight2);
						// 							solver.AddSysElement(rowIdx, vertexNum + v1 ,				-dd*weight2 );
						// 							solver.AddSysElement(rowIdx, v2             ,				 cc*weight2 );
						// 							solver.AddSysElement(rowIdx, vertexNum + v2 ,				 dd*weight2 );
						// 							rowIdx++;
					}


				}
				center_edge_index++;

				// 					for(int k = 0 ; k < 3 ; k++)
				// 					{
				// 						float m = resize_ratio_y*(referenceList[vList[k+1]].y - referenceList[vList[k]].y) / (resize_ratio_x*(referenceList[vList[k+1]].x - referenceList[vList[k]].x));
				// 						solver.AddSysElement(rowIdx  , vertexNum + vList[k+1]   ,	 weight2 );
				// 						solver.AddSysElement(rowIdx++, vertexNum + vList[k]     ,	-weight2 );
				// 						solver.AddSysElement(rowIdx  , vList[k+1]               ,	 weight2 );
				// 						solver.AddSysElement(rowIdx++, vList[k]                 ,	-weight2 );
				// 
				// 					}

				// 				}
				// 				else
				// 				{
				// 					int v1 = center_edge_back.x;
				// 					int v2 = center_edge_back.y;
				// 					double dx = referenceList[v1].x  - referenceList[v2].x;
				// 					double dy = referenceList[v1].y  - referenceList[v2].y;
				// 					double center_delta = -dx * dx - dy * dy; 
				// 					double SR[2][2];
				// 					SR[0][0] = -dx / (center_delta+0.00001);
				// 					SR[0][1] = -dy / (center_delta+0.00001);
				// 					SR[1][0] = -dy / (center_delta+0.00001);
				// 					SR[1][1] =  dx / (center_delta+0.00001);
				// 
				// 					int vList[4] = {faceList[i].x , faceList[i].y , faceList[i].z , faceList[i].x};
				// 
				// 					float weight = 1.0 * nonimportantpatch;
				// 
				// 
				// 					for(int k = 0 ; k < 3 ; k++)
				// 					{
				// 						vector2 temp_edge;
				// 						temp_edge.set((referenceList[vList[k]].x - referenceList[vList[k+1]].x) , (referenceList[vList[k]].y - referenceList[vList[k+1]].y));
				// 						double _S =  SR[0][0] * temp_edge.x + SR[0][1] * temp_edge.y;
				// 						double _R =  SR[1][0] * temp_edge.x + SR[1][1] * temp_edge.y;
				// 
				// 						solver.AddSysElement(rowIdx, vList[k]       ,	 weight );
				// 						solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight );
				// 						solver.AddSysElement(rowIdx, v1             ,	-_S*weight);
				// 						solver.AddSysElement(rowIdx, vertexNum + v1 ,	-_R*weight );
				// 						solver.AddSysElement(rowIdx, v2             ,	 _S*weight );
				// 						solver.AddSysElement(rowIdx, vertexNum + v2 ,	 _R*weight );
				// 
				// 						rowIdx++;
				// 
				// 						solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight );
				// 						solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight );
				// 						solver.AddSysElement(rowIdx, v1                         ,	 _R*weight );
				// 						solver.AddSysElement(rowIdx, vertexNum + v1             ,	-_S*weight );
				// 						solver.AddSysElement(rowIdx, v2                         ,	-_R*weight );
				// 						solver.AddSysElement(rowIdx, vertexNum + v2             ,	 _S*weight );
				// 
				// 						rowIdx++;
				// 					}
				// 
				// 					float resize_ratio_x = vertexList[rbIdx].x / referenceList[rbIdx].x;
				// 					float resize_ratio_y = vertexList[rbIdx].y / referenceList[rbIdx].y;
				// 
				// 					float weight2 = 5.0 * nonimportantpatch;
				// 
				// 					dx = (referenceList[v1].x  - referenceList[v2].x)*resize_ratio_x;
				// 					dy = (referenceList[v1].y  - referenceList[v2].y)*resize_ratio_y;
				// 					center_delta = -dx * dx - dy * dy; 
				// 
				// 					SR[0][0] = -dx / (center_delta+0.00001);
				// 					SR[0][1] = -dy / (center_delta+0.00001);
				// 					SR[1][0] = -dy / (center_delta+0.00001);
				// 					SR[1][1] =  dx / (center_delta+0.00001);
				// 
				// 					for(int k = 0 ; k < 3 ; k++)
				// 					{
				// 						vector2 temp_edge;
				// 						temp_edge.set(resize_ratio_x*(referenceList[vList[k]].x - referenceList[vList[k+1]].x) , resize_ratio_y*(referenceList[vList[k]].y - referenceList[vList[k+1]].y));
				// 						double _S =  SR[0][0] * temp_edge.x + SR[0][1] * temp_edge.y;
				// 						double _R =  SR[1][0] * temp_edge.x + SR[1][1] * temp_edge.y;
				// 
				// 						solver.AddSysElement(rowIdx, vList[k]       ,	 weight2 );
				// 						solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight2 );
				// 						solver.AddSysElement(rowIdx, v1             ,	-_S*weight2);
				// 						solver.AddSysElement(rowIdx, vertexNum + v1 ,	-_R*weight2 );
				// 						solver.AddSysElement(rowIdx, v2             ,	 _S*weight2 );
				// 						solver.AddSysElement(rowIdx, vertexNum + v2 ,	 _R*weight2 );
				// 
				// 						rowIdx++;
				// 
				// 						solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight2 );
				// 						solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight2 );
				// 						solver.AddSysElement(rowIdx, v1                         ,	 _R*weight2 );
				// 						solver.AddSysElement(rowIdx, vertexNum + v1             ,	-_S*weight2 );
				// 						solver.AddSysElement(rowIdx, v2                         ,	-_R*weight2 );
				// 						solver.AddSysElement(rowIdx, vertexNum + v2             ,	 _S*weight2 );
				// 
				// 						rowIdx++;
				// 					}
				// 					for(int k = 0 ; k < 3 ; k++)
				// 					{
				// 						float m = resize_ratio_y*(referenceList[vList[k+1]].y - referenceList[vList[k]].y) / (resize_ratio_x*(referenceList[vList[k+1]].x - referenceList[vList[k]].x));
				// 						solver.AddSysElement(rowIdx  , vertexNum + vList[k+1]   ,	 weight2 );
				// 						solver.AddSysElement(rowIdx++, vertexNum + vList[k]     ,	-weight2 );
				// 						solver.AddSysElement(rowIdx  , vList[k+1]               ,	 weight2 );
				// 						solver.AddSysElement(rowIdx++, vList[k]                 ,	-weight2 );
				// 
				// 					}

			}
		}

		//		}



		for (unsigned int i = 0 ; i < xBoundaryNum ; ++i) {
			solver.AddSysElement(rowIdx++ , x_boundary[i] , hardConst);
		}
		for (unsigned int i = 0 ; i < yBoundaryNum ; ++i) {
			solver.AddSysElement(rowIdx++ , vertexNum + y_boundary[i] , hardConst);
		}	 

		for (unsigned int i = 0 ; i < flippedConstraintNum ; ++i)
		{
			float weight = 1.0f;
			int index =flippedConstraintList[i];
			int sourceIdx = edgeList[index].x;
			int targetIdx = edgeList[index].y;
			solver.AddSysElement(rowIdx, sourceIdx, weight);				// x
			solver.AddSysElement(rowIdx++, targetIdx, -weight);
			solver.AddSysElement(rowIdx, vertexNum + sourceIdx, weight);	// y
			solver.AddSysElement(rowIdx++, vertexNum + targetIdx, -weight);
		}


		for (unsigned int i = 0 ; i < detectedge.size() ; ++i)
		{

			for(unsigned int j = 0 ; j < detectedge[i].size() ; ++j)
			{
				int index_1 = detectedge[i][j].x;
				int index_2 = detectedge[i][j].y;


				float weight = 3.0f ;
				float x_weight = weight*edge_normal[i];
				float y_weight = weight;

				solver.AddSysElement(rowIdx , index_1 , -x_weight);					//x
				solver.AddSysElement(rowIdx , index_2 , x_weight);
				solver.AddSysElement(rowIdx , vertexNum + index_1 , y_weight);		//y
				solver.AddSysElement(rowIdx++ , vertexNum + index_2 , -y_weight);


			}
		}
		for (unsigned int i = 0 ; i < vertex_table.size() ; ++i )	
		{

			if(vertex_table[i].size() >= 3)
			{
				solver.AddSysElement(rowIdx    , i,     structedgeweight);
				for(int j = 0 ; j < vertex_table[i].size() ; ++j)
				{
					solver.AddSysElement(rowIdx    , vertex_table[i][j],        -structedgeweight/vertex_table[i].size());
				}
				rowIdx++;

				solver.AddSysElement(rowIdx    , vertexNum + i,     structedgeweight);
				for(int j = 0 ; j < vertex_table[i].size() ; ++j)
				{
					solver.AddSysElement(rowIdx    , vertexNum + vertex_table[i][j],        -structedgeweight/vertex_table[i].size());
				}
				rowIdx++;
			}

		}
		for (unsigned int i = 0 ; i < patch_num ; ++i)
		{
			if(patch[i].path_id != -1 && patch[i].triangle.size() > 0)
			{
				int v1 = patch_center_edge[i].x;
				int v2 = patch_center_edge[i].y;
				float weight = 1.0f/*patch[i].saliency*/;
				double fixedge_x = referenceList[x_boundary[0]].x  - referenceList[x_boundary[1]].x;
				double fixedge_y = referenceList[x_boundary[0]].y  - referenceList[x_boundary[1]].y;

				double dx = referenceList[v1].x  - referenceList[v2].x;
				double dy = referenceList[v1].y  - referenceList[v2].y;
				double costhta = (fixedge_x * dx + fixedge_y * dy) / (sqrt(fixedge_x*fixedge_x + fixedge_y*fixedge_y) + sqrt(dx*dx+dy*dy)); 
				int vList1[2] = {v1, v2};
				int vList[2] = {x_boundary[0], x_boundary[1]};
				solver.AddSysElement(rowIdx,	   vList1[0],				  orientationweight*costhta*weight );  
				solver.AddSysElement(rowIdx,	   vList1[1],				 -orientationweight*costhta*weight );
				solver.AddSysElement(rowIdx,       vList[0],        		         -orientationweight*weight );
				solver.AddSysElement(rowIdx++,     vList[1],        		          orientationweight*weight );
				solver.AddSysElement(rowIdx,     vertexNum + vList1[0],	      orientationweight*costhta*weight );  
				solver.AddSysElement(rowIdx,     vertexNum + vList1[1],	     -orientationweight*costhta*weight );
				solver.AddSysElement(rowIdx,     vertexNum + vList[0] ,	             -orientationweight*weight );
				solver.AddSysElement(rowIdx++,   vertexNum + vList[1] ,               orientationweight*weight );			
			}
		}

	}





	// ------------------------------------ set right hand size matrix --------------------------------
	rowIdx = 0;

	// 	for (unsigned int i = 0 ; i < 6*faceNum ; ++i)
	// 	{
	// 		B[rowIdx++] = 0.0f;
	// 	}

	// 	if(Mask_Image)
	// 	{
	for(int i = 0 ; i < 12*triangle_count ; ++i)
	{
		B[rowIdx++] = 0.0f;

		// 				int vList[4] = {faceList[i].x , faceList[i].y , faceList[i].z , faceList[i].x};
		// 
		// 				float resize_ratio_x = vertexList[rbIdx].x / referenceList[rbIdx].x;
		// 				float resize_ratio_y = vertexList[rbIdx].y / referenceList[rbIdx].y;
		// 
		// 				float weight2 = 0.25 * importantpatch;
		// 
		// 				for(int k = 0 ; k < 3 ; k++)
		// 				{
		// 					float m_y = resize_ratio_y*(referenceList[vList[k+1]].y - referenceList[vList[k]].y);
		// 					float m_x = resize_ratio_x*(referenceList[vList[k+1]].x - referenceList[vList[k]].x);
		// 					B[rowIdx++] = weight2*m_y;
		// 					B[rowIdx++] = weight2*m_x;
		// 				}


		// 				int vList[4] = {faceList[i].x , faceList[i].y , faceList[i].z , faceList[i].x};
		// 
		// 				float resize_ratio_x = vertexList[rbIdx].x / referenceList[rbIdx].x;
		// 				float resize_ratio_y = vertexList[rbIdx].y / referenceList[rbIdx].y;
		// 
		// 				float weight2 = 1.0 * nonimportantpatch;
		// 
		// 				for(int k = 0 ; k < 3 ; k++)
		// 				{
		// 					float m_y = resize_ratio_y*(referenceList[vList[k+1]].y - referenceList[vList[k]].y);
		// 					float m_x = resize_ratio_x*(referenceList[vList[k+1]].x - referenceList[vList[k]].x);
		// 					B[rowIdx++] = weight2*m_y;
		// 					B[rowIdx++] = weight2*m_x;
		// 				}
	}
	//	}

	for (unsigned int i = 0 ; i < xBoundaryNum ; ++i)	// x
	{
		if(referenceList[x_boundary[i]].x == referenceList[0].x)
			B[rowIdx++] = hardConst * 0.0f;
		else
			B[rowIdx++] = hardConst * vertexList[rbIdx].x;
	}
	for (unsigned int i = 0 ; i < yBoundaryNum ; ++i)	// y
	{
		if(referenceList[y_boundary[i]].y == referenceList[0].y)
			B[rowIdx++] = hardConst * 0.0f;
		else
			B[rowIdx++] = hardConst * vertexList[rbIdx].y;
	}

	for (unsigned int i = 0 ; i < flippedConstraintNum ; ++i)
	{
		float weight = 1.0f;
		int index =flippedConstraintList[i];
		int sourceIdx = edgeList[index].x;
		int targetIdx = edgeList[index].y;
		vector2 vec = referenceList[sourceIdx] - referenceList[targetIdx];
		B[rowIdx++] = weight * 0.1 * vec.x;
		B[rowIdx++] = weight * 0.1 * vec.y;
	}

	for (unsigned int i = 0 ; i < detectedge.size() ; ++i)
	{
		for(unsigned int j = 0 ; j < detectedge[i].size() ; ++j)
		{
			B[rowIdx++] = 0.0;

		}
	}

	for(unsigned int i = 0 ; i < vertex_table.size() ; ++i)
	{
		if(vertex_table[i].size() >=3)
		{
			vector2 center(0.0,0.0);
			for(int j = 0 ; j < vertex_table[i].size() ; ++j)
			{
				center.x += referenceList[vertex_table[i][j]].x;
				center.y += referenceList[vertex_table[i][j]].y;
			}
			center /= vertex_table[i].size();
			B[rowIdx++] = structedgeweight*(referenceList[i].x - center.x);
			B[rowIdx++] = structedgeweight*(referenceList[i].y - center.y);
		}
	}

	for (unsigned int i = 0 ; i < 2*patch_count ; ++i)
	{
		B[rowIdx++] = 0.0f;

	}




	// Set right hand size matrix
	solver.SetRightHandSideMatrix(B);

	// ------------------------------------------ initial guess ---------------------------------------
	rowIdx = 0;

	for (int i = 0 ; i < vertexNum ; i++)
	{
		x[rowIdx] =  vertexList[i].x;
		x[vertexNum + rowIdx] = vertexList[i].y;
		rowIdx++;
	}

	solver.SetInitialGuess(x);


	// ---------------------------------------------- solve -------------------------------------------
	solver.solve();

	backupVertex.clear();
	rowIdx = 0;
	for(int i = 0 ; i < vertexNum ; i++)
	{
		vector2 pos(solver.GetSolution(rowIdx) , solver.GetSolution(vertexNum + rowIdx));

		//printf("(%f %f) => (%f %f)\n",referenceList[i].x,referenceList[i].y,temp.x,temp.y);
		backupVertex.push_back(pos);

		rowIdx++;
	}
}

void manager_image::faceSelection(vector2 pos)
{
	for(int i = 0 ; i < FaceSelect.size() ; ++i)
	{
		if(!FaceSelect[i])
		{
			int vList[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};
			if(triangle) 
			{
				if(IsInTriangle(vertexList[vList[0]],vertexList[vList[1]],vertexList[vList[2]],pos))
				{
					FaceSelect[i] = true;
				}
			}
			else
			{
				if(IsInQuad(vertexList[vList[0]],vertexList[vList[1]],vertexList[vList[2]],vertexList[vList[3]],pos))
				{
					FaceSelect[i] = true;
				}
			}
		}
	}
}

void manager_image::cluster()
{
	if(triangle)
	{
		cluster_index.resize(faceList.size());

		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			cluster_index[i] = 0;

			int left = Image_width, right = 0;
			int up = Image_height, down = 0;
			int Idx[3] = {faceList[i].x,faceList[i].y,faceList[i].z};

			for(unsigned k = 0; k < 3; ++k) {
				if(vertexList[Idx[k]].x < left)		left = vertexList[Idx[k]].x;
				if(vertexList[Idx[k]].x > right)	right = vertexList[Idx[k]].x;
				if(vertexList[Idx[k]].y < up)		up = vertexList[Idx[k]].y;
				if(vertexList[Idx[k]].y > down)		down = vertexList[Idx[k]].y;
			}

			if (right > Image_width - 1)		right = Image_width - 1;
			if (down > Image_height - 1)		down = Image_height - 1;

			int count = 0;
			faceflexibleList[i] = 0.0f;

			for(unsigned m = left; m <= right; ++m) {
				for(unsigned n = up; n <= down; ++n) {
					// 檢查是否落於triangle內
					if(IsInTriangle(vertexList[Idx[0]], vertexList[Idx[1]], vertexList[Idx[2]], vector2(m, n)) == true) {

						uchar color = Mask_Image->imageData[n*Mask_Image->widthStep + 3*m];

						if(color == 255) cluster_index[i] = 1;

					}
				}
			}

		}
	}
	else
	{
		cluster_index.resize(faceList.size());
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			cluster_index[i] = 0;
		}

		for (int j = 0 ; j < Image_width ; j++)
		{
			for (int k = 0 ; k < Image_height ; k++)
			{
				int xIdx = j / xQuadSize;
				int yIdx = k / yQuadSize;
				int qIdx = yIdx * xGridSize + xIdx;

				uchar color = Mask_Image->imageData[k*Mask_Image->widthStep + 3*j];

				if(color == 255) cluster_index[qIdx] = 1;

			}
		}


	}
}

void manager_image::FindPatchCenterEdge()
{
	patch_center_edge.resize(patch_num);
	for(int i = 0 ; i < patch_num ; ++i) {
		patch_center_edge[i].x = 0;
		patch_center_edge[i].y = 0;
	}

	if(triangle) {
		for(int i = 0 ; i < patch_num ; i++) {
			if(patch[i].path_id != -1 && patch[i].triangle.size() > 0) {
				int patch_triangle_size = patch[i].triangle.size();

				int lt_x = Image_width;
				int lt_y = Image_height;
				int rb_x = 0;
				int rb_y = 0;

				for(int p = 0 ; p < patch_triangle_size ; p++)
				{
					int face_index = patch[i].triangle[p];
					int vList[3] = {faceList[face_index].x , faceList[face_index].y , faceList[face_index].z};

					for(int k = 0 ; k < 3 ; k++)
					{
						if(referenceList[vList[k]].x < lt_x) lt_x = referenceList[vList[k]].x;
						if(referenceList[vList[k]].x > rb_x) rb_x = referenceList[vList[k]].x;
						if(referenceList[vList[k]].x < lt_y) lt_y = referenceList[vList[k]].y;
						if(referenceList[vList[k]].x < rb_y) lt_y = referenceList[vList[k]].y;
					}

				}

				int patch_center_x = (lt_x + rb_x) / 2; 
				int patch_center_y = (lt_y + rb_y) / 2;

				double min_length = Image_width * Image_height;
				int v1 , v2;

				for(int p = 0 ; p < patch_triangle_size ; p++)
				{
					int face_index = patch[i].triangle[p];
					int vList[3] = {faceList[face_index].x , faceList[face_index].y , faceList[face_index].z};

					for(int k = 0 ; k < 3 ; k++)
					{
						vector2 d;
						if(k != 2) d.set(referenceList[vList[k]].x - referenceList[vList[k+1]].x , referenceList[vList[k]].y - referenceList[vList[k+1]].y);
						else       d.set(referenceList[vList[k]].x - referenceList[vList[0]].x , referenceList[vList[k]].y - referenceList[vList[0]].y);
						double m ;
						if(d.x == 0) m = -d.y / 0.001;
						else         m = -d.y / d.x;
						double c = -(m*referenceList[vList[k]].x + referenceList[vList[k]].y);
						double length = abs(m*referenceList[vList[k]].x + referenceList[vList[k]].y + c) / sqrt(m*m+1);
						if(length < min_length)
						{

							min_length = length;
							if(k != 2)
							{
								v1 = vList[k];
								v2 = vList[k+1];
							}
							else
							{
								v1 = vList[2];
								v2 = vList[0];
							}


						}
					}

				}

				vector2  temp;
				temp.set(v1 , v2);
				patch_center_edge[i] = temp;
				//patch_center_edge.push_back(temp);
			}
		}
	} else {
		int maxCenterEdge = 0;
		vector2 maxCenter;
		for(int i = 0 ; i < patch_num ; i++) {
			if(patch[i].path_id != -1 && patch[i].quad.size() > 0) {
				int patch_quad_size = patch[i].quad.size();
				int lt_x = Image_width;
				int lt_y = Image_height;
				int rb_x = 0;
				int rb_y = 0;

				for(int p = 0 ; p < patch_quad_size ; p++) {
					int face_index = patch[i].quad[p];
					int vList[4] = {faceList[face_index].x , faceList[face_index].y , faceList[face_index].z,faceList[face_index].w};

					for(int k = 0 ; k < 4 ; k++) {
						if(referenceList[vList[k]].x < lt_x) lt_x = referenceList[vList[k]].x;
						if(referenceList[vList[k]].x > rb_x) rb_x = referenceList[vList[k]].x;
						if(referenceList[vList[k]].x < lt_y) lt_y = referenceList[vList[k]].y;
						if(referenceList[vList[k]].x < rb_y) lt_y = referenceList[vList[k]].y;
					}
				}

				int patch_center_x = (lt_x + rb_x) / 2; 
				int patch_center_y = (lt_y + rb_y) / 2;

				double min_length = Image_width * Image_height;
				int v1 , v2;

				v1 = 0; 
				v2 = 0;
				for(int p = 0 ; p < patch_quad_size ; p++) {
					int face_index = patch[i].quad[p];

					int vList[5] = {faceList[face_index].x , faceList[face_index].y , faceList[face_index].z,faceList[face_index].w, faceList[face_index].x};

					for(int k = 0 ; k < 4 ; k++) {
						vector2 d;
						d.set(referenceList[vList[k]].x - referenceList[vList[k+1]].x , referenceList[vList[k]].y - referenceList[vList[k+1]].y);
						double m ;
						if(d.x == 0) {
							m = -d.y / 0.001;
						} else {
							m = -d.y / d.x;
						}
						double c = -(m*referenceList[vList[k]].x + referenceList[vList[k]].y);
						double length = abs(m*referenceList[vList[k]].x + referenceList[vList[k]].y + c) / sqrt(m*m+1);
						//double length = abs(-d.y * patch_center_x + d.x * patch_center_y + c * d.x) / sqrt(d.y*d.y+d.x*d.x);
						// avoid cols : 0 
						/*if( vList[k] >= xGridSize+1 && vList[k] <= vertexNum-(xGridSize+1) && (vList[k]+1) % (xGridSize+1) != 0 && (vList[k]) % (xGridSize+1) != 0
						&&  vList[k+1] >= xGridSize+1 && vList[k+1] <= vertexNum-(xGridSize+1) && (vList[k+1]+1) % (xGridSize+1) != 0 && (vList[k+1]) % (xGridSize+1) != 0
						) {*/
						if(length < min_length) {
							min_length = length;
							if(k != 2) {
								v1 = vList[k];
								v2 = vList[k+1];
							} else {
								v1 = vList[2];
								v2 = vList[0];
							}
						}
						//}
					}
				}
				/*
				if(v2 < v1) {
				int tmp = v1;
				v1 = v2;
				v2 = tmp;
				}
				*/
				cout << i << " " << v1 <<  " " << v2 << " " << patch[i].saliency << endl;
				vector2  temp;
				temp.set(v1 , v2);
				patch_center_edge[i] = temp;

				if(maxCenterEdge < patch[i].saliency) {
					maxCenterEdge = patch[i].saliency;
					maxCenter.set(v1,v2);
				}

				//patch_center_edge.push_back(temp);
			}
		}

		for(int i = 0 ; i < patch_center_edge.size() ; i++) {
			//	patch_center_edge[i] = maxCenter;
		}

	}

}

void manager_image::PatchSaliency()
{
	for(int i = 0 ; i < patch_num ; i++)
	{
		patch[i].saliency = 0;
	}
	for(int j  = 0 ; j < Image_height ; j++)
	{
		for(int i = 0 ; i < Image_width ; i++)
		{
			int index = cluster_index[j*Image_width + i];
			uchar s = Saliency_Map->imageData[j*Saliency_Map->widthStep + 3*i /*+ 2*/];
			patch[index].saliency += double(s)/255;
		}
	}
	double s_max = 0.0;
	double s_min = 1.0;
	for(int i = 0 ; i < patch_num ; i++)
	{
		patch[i].saliency = pow(patch[i].saliency / patch[i].number, 1);
		if(patch[i].saliency < s_min) s_min = patch[i].saliency;
		if(patch[i].saliency > s_max) s_max = patch[i].saliency;
	}

	double d_s = s_max - s_min;
	//     int p = 0;
	// 	sortsaliency.resize(patch_num);
	for(int i = 0 ; i < patch_num ; i++)
	{
		patch[i].saliency = ((patch[i].saliency - s_min) / d_s);
		/*		sortsaliency[p++] = patch[i].saliency;*/
	}
	/* 	sort(sortsaliency.begin(),sortsaliency.end());*/
	// 	for(int i = 0 ; i < patch_num ; i++)
	// 	printf("%f %d\n",sortsaliency[i],i);

}
void manager_image::FindCenterEdge()
{


	int face_size =faceList.size();

	int lt_x = Image_width;
	int lt_y = Image_height;
	int rb_x = 0;
	int rb_y = 0;

	for(int p = 0 ; p < face_size ; p++)
	{
		int vList[3] = {faceList[p].x , faceList[p].y , faceList[p].z};

		if(cluster_index[p] == 1)
		{
			for(int k = 0 ; k < 3 ; k++)
			{
				if(referenceList[vList[k]].x < lt_x) lt_x = referenceList[vList[k]].x;
				if(referenceList[vList[k]].x > rb_x) rb_x = referenceList[vList[k]].x;
				if(referenceList[vList[k]].x < lt_y) lt_y = referenceList[vList[k]].y;
				if(referenceList[vList[k]].x < rb_y) lt_y = referenceList[vList[k]].y;
			}
		}

	}

	int patch_center_x = (lt_x + rb_x) / 2; 
	int patch_center_y = (lt_y + rb_y) / 2;

	double min_length = Image_width * Image_height;
	int v1 = 0;
	int v2 = 0;

	for(int p = 0 ; p < face_size ; p++)
	{
		int vList[3] = {faceList[p].x , faceList[p].y , faceList[p].z};

		if(cluster_index[p] == 1)
		{
			for(int k = 0 ; k < 3 ; k++)
			{
				vector2 d;
				if(k != 2) d.set(referenceList[vList[k]].x - referenceList[vList[k+1]].x , referenceList[vList[k]].y - referenceList[vList[k+1]].y);
				else       d.set(referenceList[vList[k]].x - referenceList[vList[0]].x , referenceList[vList[k]].y - referenceList[vList[0]].y);
				double m ;
				if(d.x == 0) m = -d.y / 0.001;
				else         m = -d.y / d.x;
				double c = -(m*referenceList[vList[k]].x + referenceList[vList[k]].y);
				double length = abs(m*referenceList[vList[k]].x + referenceList[vList[k]].y + c) / sqrt(m*m+1);
				if(length < min_length)
				{

					min_length = length;
					if(k != 2)
					{
						v1 = vList[k];
						v2 = vList[k+1];
					}
					else
					{
						v1 = vList[2];
						v2 = vList[0];
					}


				}
			}
		}


	}

	vector2  temp;
	temp.set(v1 , v2);
	center_edge = temp;

}

void manager_image::FindCenterEdgeB()
{


	int face_size =faceList.size();

	int lt_x = Image_width;
	int lt_y = Image_height;
	int rb_x = 0;
	int rb_y = 0;

	for(int p = 0 ; p < face_size ; p++)
	{
		int vList[3] = {faceList[p].x , faceList[p].y , faceList[p].z};

		if(cluster_index[p] == 0)
		{
			for(int k = 0 ; k < 3 ; k++)
			{
				if(referenceList[vList[k]].x < lt_x) lt_x = referenceList[vList[k]].x;
				if(referenceList[vList[k]].x > rb_x) rb_x = referenceList[vList[k]].x;
				if(referenceList[vList[k]].x < lt_y) lt_y = referenceList[vList[k]].y;
				if(referenceList[vList[k]].x < rb_y) lt_y = referenceList[vList[k]].y;
			}
		}

	}

	int patch_center_x = (lt_x + rb_x) / 2; 
	int patch_center_y = (lt_y + rb_y) / 2;

	double min_length = Image_width * Image_height;
	int v1 = 0;
	int v2 = 0;

	for(int p = 0 ; p < face_size ; p++)
	{
		int vList[3] = {faceList[p].x , faceList[p].y , faceList[p].z};

		if(cluster_index[p] == 0)
		{
			for(int k = 0 ; k < 3 ; k++)
			{
				vector2 d;
				if(k != 2) d.set(referenceList[vList[k]].x - referenceList[vList[k+1]].x , referenceList[vList[k]].y - referenceList[vList[k+1]].y);
				else       d.set(referenceList[vList[k]].x - referenceList[vList[0]].x , referenceList[vList[k]].y - referenceList[vList[0]].y);
				double m ;
				if(d.x == 0) m = -d.y / 0.001;
				else         m = -d.y / d.x;
				double c = -(m*referenceList[vList[k]].x + referenceList[vList[k]].y);
				double length = abs(m*referenceList[vList[k]].x + referenceList[vList[k]].y + c) / sqrt(m*m+1);
				if(length < min_length)
				{

					min_length = length;
					if(k != 2)
					{
						v1 = vList[k];
						v2 = vList[k+1];
					}
					else
					{
						v1 = vList[2];
						v2 = vList[0];
					}


				}
			}
		}


	}

	vector2  temp;
	temp.set(v1 , v2);
	center_edge_back = temp;

}

bool UDgreater(vector<vector2> elem1, vector<vector2> elem2) 
{
	float length1 = (elem1[0]-elem1[1]).length();
	float length2 = (elem2[0]-elem2[1]).length();
	return length1 > length2;
}

void manager_image::EdgeDetection()
{
	constraintLines.clear();
	curConstraintLine.clear();
	edgeSlopeConstraintList.clear();

	const float minEdgeLength = 5.0f;		// min edge length
	const float minEdgeDistance = 5.0f;	// min edge distance
	const unsigned int maxEdgeNum = 30;		// max edge number

	// OpenCV Canny Edge Detection
	IplImage* dst = cvCreateImage( cvGetSize(Original_Image), 8, 1 );
	IplImage* color_dst = cvCreateImage( cvGetSize(Original_Image), 8, 3 );
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;

	cvCvtColor(Original_Image,dst, CV_BGR2GRAY);
	cvCanny(dst, dst, 478, 505, 3);
	cvCvtColor(dst, color_dst, CV_GRAY2BGR);
	lines = cvHoughLines2(dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 80, 30, 10);

	// sort detected edge by length desc
	vector<vector<vector2>> orderedLines;
	for (unsigned i = 0 ; i < lines->total ; ++i)
	{
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
		vector<vector2> curLine;
		curLine.push_back(vector2(line[0].x, line[0].y));
		curLine.push_back(vector2(line[1].x, line[1].y));
		orderedLines.push_back(curLine);
	}
	sort(orderedLines.begin(), orderedLines.end(), UDgreater);

	// check edge length and wheather too close to above edge
	for (unsigned i = 0 ; i < orderedLines.size() ; ++i)
	{
		vector2 startVertex(orderedLines[i][0]);
		vector2 endVertex(orderedLines[i][1]);
		float edgeLength = (endVertex-startVertex).length();
		if (edgeLength >= minEdgeLength && CheckValidLine(startVertex, endVertex, minEdgeDistance))
		{
			SetCurConstrainLine(startVertex);
			SetCurConstrainLine(endVertex);
			AddConstrainLine(false);
		}
	}

	// automatic edge detection may too much constraint line, select the 30 longest line only
	if (constraintLines.size() > maxEdgeNum)
		constraintLines.resize(maxEdgeNum);


}

//--- check if current line is valid (not too close exist line) 
bool manager_image::CheckValidLine(vector2 startVertex, vector2 endVertex, const float minEdgeDistance)
{
	vector2 centerVertex = (startVertex+endVertex)/2;

	// boundary line need not to be line constraint
	if (startVertex.x == 0 && endVertex.x == 0 || 
		startVertex.x == Image_width-1 && endVertex.x == Image_width-1 || 
		startVertex.y == 0 && endVertex.y == 0 || 
		startVertex.y == Image_height-1 && endVertex.y == Image_height-1)
	{
		return false;
	}


	// if edge distance smaller than min edge distance, it invalid
	for (int i = 0 ; i < constraintLines.size() ; ++i)
	{
		for (int j = 0 ; j < constraintLines[i].size()-1 ; ++j)
		{
			bool new2old = CheckValidCenter(centerVertex, constraintLines[i][j], constraintLines[i][j+1], minEdgeDistance);
			bool old2new = CheckValidCenter((constraintLines[i][j]+constraintLines[i][j+1])/2.0f, startVertex, endVertex, minEdgeDistance);
			if (!new2old || !old2new)
			{
				return false;
			}
		}
	}

	return true;
}

bool manager_image::CheckValidCenter(vector2 q, vector2 p0, vector2 p1, const float minEdgeDistance)
{
	vector2 projectVertex;
	float d = 0;
	if (p0.y == p1.y)	// horizontal line
	{
		projectVertex.set(q.x, p0.y);
	}
	else if(p0.x == p1.x)	// vertical line
	{
		projectVertex.set(p0.x, q.y);
	}
	else
	{
		IterativeSolver solver;
		float B[2] = {0.0f, 0.0f};
		float x[2] = {0.0f, 0.0f};
		solver.Create(2, 2);
		solver.AddSysElement(0, 0, p1.x-p0.x);
		solver.AddSysElement(0, 1, p1.y-p0.y);
		solver.AddSysElement(1, 0, p0.y-p1.y);
		solver.AddSysElement(1, 1, p1.x-p0.x);
		B[0] = -(-q.x*(p1.x-p0.x)-q.y*(p1.y-p0.y));
		B[1] = -(-p0.y*(p1.x-p0.x)+p0.x*(p1.y-p0.y));
		solver.SetRightHandSideMatrix(B);
		solver.SetInitialGuess(x);
		solver.solve();
		projectVertex.set(solver.GetSolution(0), solver.GetSolution(1));
	}

	if (projectVertex.x >= (p0.x < p1.x ? p0.x : p1.x) && 
		projectVertex.x <= (p0.x > p1.x ? p0.x : p1.x) && 
		projectVertex.y >= (p0.y < p1.y ? p0.y : p1.y) && 
		projectVertex.y <= (p0.y > p1.y ? p0.y : p1.y))
	{
		d = (q-projectVertex).length();
		if (d < minEdgeDistance)
		{
			return false;
		}
	}
	//cout << projectVertex.x << " " << projectVertex.y << endl;
	return true;
}

//--- set current constrain line vertex
void manager_image::SetCurConstrainLine(vector2 pos) {
	bool isDuplicate = false;

	// check if vertex already in constraintLines
	for(vector<vector<vector2>>::size_type i = 0; i < constraintLines.size(); ++i) {
		for(vector<vector2>::size_type j = 0; j < constraintLines[i].size(); ++j) {
			if(constraintLines[i][j].x == pos.x && constraintLines[i][j].y == pos.y) {
				isDuplicate = true;
				break;
			}
		}
	}
	// check if vertex already in curConstraintLine
	for(vector<vector2>::size_type i = 0; i < curConstraintLine.size(); ++i) {
		if(curConstraintLine[i].x == pos.x && curConstraintLine[i].y == pos.y) {
			isDuplicate = true;
			break;
		}
	}
	// if no duplicate vertex, push into current constrain line
	if(isDuplicate == false) {
		curConstraintLine.push_back(pos);
	}
}

//--- add current constrain line into constrain line
void manager_image::AddConstrainLine(bool initialize) {
	// at least two vertex
	if(curConstraintLine.size() >= 2) {
		constraintLines.push_back(curConstraintLine);

	}
	curConstraintLine.clear();
}


void manager_image::updateedgeslope()
{
	vector<vector<float>> inslope;
	vector<float>         stdslope;
	edge_normal.resize(detectedge.size());
	inslope.resize(detectedge.size());


	for (unsigned int i = 0 ; i < detectedge.size() ; ++i)
	{
		float sumslope = 0.0f;
		inslope[i].resize(detectedge[i].size());
		for(unsigned int j = 0 ; j < detectedge[i].size() ; ++j)
		{

			//int j = detectedge[i].size() / 2;

			float index_1 = detectedge[i][j].x;
			float index_2 = detectedge[i][j].y;

			vector2 p1 = vertexList[index_1];
			vector2 p2 = vertexList[index_2];
			inslope[i][j] = (p2.y - p1.y) / ( p2.x - p1.x + 0.0001);
			sumslope +=  inslope[i][j];

		}
		edge_normal[i] = sumslope / (detectedge[i].size()+1) ;
	}
	float sumstdmean =0.0;
	stdslope.resize(detectedge.size());
	for (unsigned int i = 0 ; i < detectedge.size() ; ++i)
	{
		float sumslopevar = 0.0;
		for(unsigned int j = 0 ; j < detectedge[i].size() ; ++j)
		{
			sumslopevar += (inslope[i][j]-edge_normal[i])*(inslope[i][j]-edge_normal[i]);
		}
		stdslope[i] = sqrt(sumslopevar / (detectedge[i].size()+1)) ;
		sumstdmean += stdslope[i];
		printf("\nstdslope[%d] = %f\n" , i , stdslope[i]);
	}
	sumstdmean = sumstdmean / (detectedge.size()+1);
	printf("\nsumstdmean = %f\n" , sumstdmean);

	inslope.clear();
}
void manager_image::Laplacian()
{
	if(triangle)
	{
		vertex_table.clear();
		vertex_table.resize(referenceList.size());
		for(int i = 0 ; i < faceList.size() ; ++i)
		{
			int vList[3] = {faceList[i].x,faceList[i].y,faceList[i].z};

			for(int j = 0 ; j < 3 ; ++j)
			{
				if(referenceList[vList[j]].x == 0 || referenceList[vList[j]].x == Image_width || referenceList[vList[j]].y == 0 || referenceList[vList[j]].y == Image_height)
				{

				}
				else
				{
					int temp_vertex[3];
					if(j == 0)
					{
						temp_vertex[0] = vList[0];
						temp_vertex[1] = vList[1];
						temp_vertex[2] = vList[2];
					}
					else if(j == 1)
					{
						temp_vertex[0] = vList[1];
						temp_vertex[1] = vList[0];
						temp_vertex[2] = vList[2];
					}
					else
					{
						temp_vertex[0] = vList[2];
						temp_vertex[1] = vList[0];
						temp_vertex[2] = vList[1];
					}

					bool flag[2] = {0};
					for(int k = 0 ; k < vertex_table[temp_vertex[0]].size() ; ++k)
					{
						if(vertex_table[temp_vertex[0]][k] == temp_vertex[1])
						{
							flag[0] = 1;
						}
						if(vertex_table[temp_vertex[0]][k] == temp_vertex[2])
						{
							flag[1] = 1;
						}
					}
					if(flag[0] == 0) vertex_table[temp_vertex[0]].push_back(temp_vertex[1]);
					if(flag[1] == 0) vertex_table[temp_vertex[0]].push_back(temp_vertex[2]);
				}

			}
		}



	}
	else
	{
		structureListCenter.clear();
		structureList.clear();
		structureList2.clear();
		structureList.resize((xGridSize - 1 )* (yGridSize - 1));
		structureList2.resize((xGridSize - 1 )* (yGridSize - 1));

		int count = 0;


		for (int j = 1 ; j <= xGridSize - 1 ; j++)
		{
			for (int k = 1 ; k <= yGridSize - 1 ; k++)
			{
				int Idx = k * (xGridSize + 1 ) + j ;

				structureList[count].x = Idx - (xGridSize + 1);
				structureList[count].y = Idx + 1;
				structureList[count].z = Idx + (xGridSize + 1);
				structureList[count].w = Idx - 1;
				structureList2[count].x = Idx - (xGridSize + 2);
				structureList2[count].y = Idx - xGridSize;
				structureList2[count].z = Idx + (xGridSize + 2);
				structureList2[count].w = Idx + xGridSize;
				structureListCenter.push_back(Idx);

				count++;
			}
		}
	}

}

template <class T>
double **manager_image::mallocArray(int rows,int cols) {
	T **a = (T **)malloc(rows*sizeof(T *));
	for(int i = 0 ; i < rows; i++) {
		a[i] = (T *)malloc(cols*sizeof(T));
	}
	return a;
}


void manager_image::QuadMeshDeformation()
{
	// quadprog init parameter
	QuadProgPP::Matrix<double> G, CE, CI;
	QuadProgPP::Matrix<double> GT;
	QuadProgPP::Matrix<double> H;

	// becare for type
	VectorQuad<double> g0, ce0, ci0, xx;

	unsigned int xBoundaryNum = x_boundary.size();
	unsigned int yBoundaryNum = y_boundary.size();
	unsigned int faceNum = faceList.size();
	unsigned int structureListCenterNum = structureListCenter.size();

	int quad_count = 0;
	int patch_count = 0;



	for (int i = 0; i < patch_num ; i++) {
		if (patch[i].path_id !=-1 && patch[i].quad.size() > 0) {
			int quad_size = patch[i].quad.size();
			quad_count +=  quad_size;
			patch_count++;
		}
	}

	// 2 means avg gridH , gridW
	const int cols = 2 * vertexNum ; 
	const int rows = 8*faceNum + 16*quad_count /*+ 2*faceNum + 2*structureListCenterNum */+ 4*faceNum /*+ 2*patch_count*/ /*+ xBoundaryNum + yBoundaryNum */;


	static float *B = NULL;
	static float *x = NULL;
	static float ***coeff = NULL;

	B = new float[rows];
	x = new float[cols];

	int rowIdx = 0;
	int temp_count = 0;

	const float hardConst = 100.0f;
	const float edgeweight = 5;
	const float structedgeweight  = 0.0f;
	const float orientationweight = 0.0f;

	// -------------------------------------- set system matrix equation start-------------------------------------
	// aspect ratio

	if (!solver.IsTheSameSize(rows, cols))
	{

		solver.ResetSolver(rows,cols);
		coeff = new float**[faceNum];
		for (unsigned int i = 0 ; i < faceNum ; ++i)
		{
			coeff[i] = new float*[8];
			for (int j = 0 ; j < 8 ; ++j)
			{
				coeff[i][j] = new float[8];
			}
		}

		vector<vector4> Ap;

		Ap.clear();
		Ap.resize(8);

		rowIdx = 0;

		//initial A
		// (height,width)
		G.resize(rows,cols);
		GT.resize(cols,rows);

		// unit matrix
		H.resize(cols,cols);

		// =
		int xCell = (xGridSize + 1);
		int yCell = (yGridSize + 1);
		int m = 2*xCell + 2*yCell;
		int y_size = 2*xCell;//;
		ce0.resize(m);
		CE.resize(cols,m);

		// >= > , <= , <
		int ci_size = cols*3 ;
		CI.resize(cols,ci_size);
		ci0.resize(ci_size);

		g0.resize(cols);
		// result
		xx.resize(cols);

		// bee ok 2014 01 08
		// initial CE equal case y
		for(int i = 0 ; i < cols; i++) {
			for(int j = 0 ; j < m ; j++) {
				CE[i][j] = 0;
				ce0[j] = 0;
			}
		}

		for(int i = 0 ; i < xCell ; i++) {
			//y
			CE[ vertexNum + i][i] = 1;
			CE[((cols-1)-xGridSize)+i][i+xCell] = 1;

			ce0[i] = 0;
			ce0[i+xCell] = -Image_height * 1;
		}

		for(int i = 0 ; i < yCell ; i++) {
			// x

			CE[i*xCell][i+y_size] = 1;  // x1
			CE[i*xCell + xGridSize][i+y_size+ yCell] = 1; // x_
			// ce0
			ce0[i+y_size] = 0;
			ce0[i+y_size + yCell] = -Image_width * 0.5;
		}

		// g0 init
		for(int i = 0 ; i < cols; i++) {
			g0[i] = 0;
			xx[i] = 0;
		}


		// OK
		// yGridSize = ycells
		// xGridSize = xcells

		for(int i = 0 ; i < cols; i++) {
			// initial ci0 = 0
			for(int j = 0 ; j < ci_size; j++) {
				CI[i][j] = 0;
				ci0[j] = 0;
			}
		}


		int inequality = 0;
		for(int i = 0 ; i < vertexNum-1; i++) {
			// x ~ x_n-1
			if((i+1) % xCell != 0) {
				CI[i][inequality] = -1;
				CI[i+1][inequality] = 1;
				inequality++;
			}
		}

		for(int i = vertexNum ; i < cols-1; i++) {
			// y_1 ~ y_n-1 
			if(i + xCell  < cols) {
				// y < final_rows
				CI[i][inequality] = -1;
				CI[i+xCell][inequality] = 1;
				inequality++;
			}
		}


		for(int i = 0 ; i < cols; i++) {
			ci0[i] = -0.5;
			if(i >= vertexNum) {
				ci0[i] = 0;
			}
			//All x,y > 0
			ci0[inequality] = 0;
			CI[i][inequality]  = 1;
			inequality++;
		}

		for(int i = 0 ; i < vertexNum; i++) {
			// All x < image_width *0.5
			ci0[inequality] = Image_width*0.5;
			CI[i][inequality]  = -1;
			inequality++;
		}	
		for(int i = 0 ; i < rows;i++) {
			for(int j = 0 ; j < cols;j++) {
				G[i][j] = 0;
			}
		}

		// -------------------------------------- set system matrix end-------------------------------------
		int center_edge_index = 0;
		for(int i = 0 ; i < patch_num ; ++i) {
			if(patch[i].path_id != -1 && patch[i].quad.size() > 0) {
				//cout << endl;
				int patch_quad_size = patch[i].quad.size();

				temp_count += patch_quad_size;

				int v1 = patch_center_edge[i].x;
				int v2 = patch_center_edge[i].y;



				double dx = referenceList[v1].x  - referenceList[v2].x;
				double dy = referenceList[v1].y  - referenceList[v2].y;
				double center_delta = -dx * dx - dy * dy; 
				double SR[2][2];
				SR[0][0] = -dx / (center_delta+0.00001);
				SR[0][1] = -dy / (center_delta+0.00001);
				SR[1][0] = -dy / (center_delta+0.00001);
				SR[1][1] =  dx / (center_delta+0.00001);

				//cout << patch[i].saliency  << " " ;
				for (int j = 0 ; j < patch_quad_size ; j++)
				{
					int face_index = patch[i].quad[j];
					int vList[5]  = {faceList[face_index].x , faceList[face_index].y , faceList[face_index].z , faceList[face_index].w,faceList[face_index].x};
					double weight = 3 *  patch[i].saliency /*faceflexibleList[face_index]*/;

					for (int j = 0 ; j < 4 ; ++j)
					{
						Ap[2 * j + 0].x = referenceList[vList[j]].x;
						Ap[2 * j + 0].y = -referenceList[vList[j]].y;
						Ap[2 * j + 0].z = 1.0f;
						Ap[2 * j + 0].w = 0.0f;

						Ap[2 * j + 1].x = referenceList[vList[j]].y;
						Ap[2 * j + 1].y = referenceList[vList[j]].x;
						Ap[2 * j + 1].z = 0.0f;
						Ap[2 * j + 1].w = 1.0f;
					}

					SimilarityCoeff(Ap, coeff[i]);  

					for (int j = 0 ; j < 8 ; ++j)
					{
						if (fabs(coeff[i][j][0]) > 0.00001f) {
							G[rowIdx][vList[0]] = weight * coeff[i][j][0];
						}
						if (fabs(coeff[i][j][1]) > 0.00001f) {
							G[rowIdx][vertexNum + vList[0]] = weight * coeff[i][j][1];
						}
						if (fabs(coeff[i][j][2]) > 0.00001f) {
							G[rowIdx][vList[1]] = weight * coeff[i][j][2];
						}
						if (fabs(coeff[i][j][3]) > 0.00001f) {
							G[rowIdx][vertexNum + vList[1]] = weight * coeff[i][j][3];
						}
						if (fabs(coeff[i][j][4]) > 0.00001f) {
							G[rowIdx][vList[2]] = weight * coeff[i][j][4];
						}
						if (fabs(coeff[i][j][5]) > 0.00001f) {
							G[rowIdx][vertexNum + vList[2]] = weight * coeff[i][j][5];
						}
						if (fabs(coeff[i][j][6]) > 0.00001f) {
							G[rowIdx][vList[3]] = weight * coeff[i][j][6];
						}
						if (fabs(coeff[i][j][7]) > 0.00001f) {
							G[rowIdx][vertexNum + vList[3]] = weight * coeff[i][j][7];
						}
						++rowIdx;
					}

					double resize_ratio_x = Image_width*0.5 / referenceList[rbIdx].x;
					double resize_ratio_y = Image_height*1 / referenceList[rbIdx].y;

					//float weight2 = 0 *  (1-patch[i].saliency)  /*(1-faceflexibleList[face_index])*/;
					weight = 0.1 *  (1-patch[i].saliency);  /*(1-faceflexibleList[face_index])*/;
					for (int j = 0 ; j < 4 ; ++j)
					{
						Ap[2 * j + 0].x = referenceList[vList[j]].x;
						Ap[2 * j + 0].y = -referenceList[vList[j]].y;
						Ap[2 * j + 0].z = 1.0f;
						Ap[2 * j + 0].w = 0.0f;

						Ap[2 * j + 1].x = referenceList[vList[j]].y;
						Ap[2 * j + 1].y = referenceList[vList[j]].x;
						Ap[2 * j + 1].z = 0.0f;
						Ap[2 * j + 1].w = 1.0f;
					}

					SimilarityCoeff(Ap, coeff[i]);  

					for (int j = 0 ; j < 8 ; ++j)
					{
						if (fabs(coeff[i][j][0]) > 0.00001f) {
							G[rowIdx][vList[0]] = weight * coeff[i][j][0];
						}
						if (fabs(coeff[i][j][1]) > 0.00001f) {
							G[rowIdx][vertexNum + vList[0]] = weight * coeff[i][j][1];
						}
						if (fabs(coeff[i][j][2]) > 0.00001f) {
							G[rowIdx][vList[1]] = weight * coeff[i][j][2];
						}
						if (fabs(coeff[i][j][3]) > 0.00001f) {
							G[rowIdx][vertexNum + vList[1]] = weight * coeff[i][j][3];
						}
						if (fabs(coeff[i][j][4]) > 0.00001f) {
							G[rowIdx][vList[2]] = weight * coeff[i][j][4];
						}
						if (fabs(coeff[i][j][5]) > 0.00001f) {
							G[rowIdx][vertexNum + vList[2]] = weight * coeff[i][j][5];
						}
						if (fabs(coeff[i][j][6]) > 0.00001f) {
							G[rowIdx][vList[3]] = weight * coeff[i][j][6];
						}
						if (fabs(coeff[i][j][7]) > 0.00001f) {
							G[rowIdx][vertexNum + vList[3]] = weight * coeff[i][j][7];
						}
						++rowIdx;
					}

				}
				center_edge_index++;
				//cout << endl;

			} else {
			}


		}
		//int center_edge_index = 0;
		//for(int i = 0 ; i < patch_num ; ++i) {
		//	if(patch[i].path_id != -1 && patch[i].quad.size() > 0) {
		//		//cout << endl;
		//		int patch_quad_size = patch[i].quad.size();

		//		temp_count += patch_quad_size;

		//		int v1 = patch_center_edge[i].x;
		//		int v2 = patch_center_edge[i].y;

		//		double dx = referenceList[v1].x  - referenceList[v2].x;
		//		double dy = referenceList[v1].y  - referenceList[v2].y;
		//		double center_delta = -dx * dx - dy * dy; 
		//		double SR[2][2];
		//		SR[0][0] = -dx / (center_delta+0.00001);
		//		SR[0][1] = -dy / (center_delta+0.00001);
		//		SR[1][0] = -dy / (center_delta+0.00001);
		//		SR[1][1] =  dx / (center_delta+0.00001);

		//		//cout << patch[i].saliency  << " " ;
		//		for (int j = 0 ; j < patch_quad_size ; j++)
		//		{
		//			int face_index = patch[i].quad[j];
		//			int vList[5]  = {faceList[face_index].x , faceList[face_index].y , faceList[face_index].z , faceList[face_index].w,faceList[face_index].x};

		//			double weight = 3 *  patch[i].saliency /*faceflexibleList[face_index]*/;
		//			if(FaceSelect[face_index]) weight = 6.0f;	


		//			for(int k = 0 ; k < 4 ; k++)
		//			{
		//				vector2 temp_edge;
		//				temp_edge.set((referenceList[vList[k]].x - referenceList[vList[k+1]].x) , (referenceList[vList[k]].y - referenceList[vList[k+1]].y));
		//				double _S =  (SR[0][0] * temp_edge.x + SR[0][1] * temp_edge.y);
		//				double _R =  (SR[1][0] * temp_edge.x + SR[1][1] * temp_edge.y);

		//				G[rowIdx][vList[k]] = weight;
		//				G[rowIdx][vList[k+1]] = -weight;
		//				/*
		//				solver.AddSysElement(rowIdx, vList[k]       ,	 weight );  
		//				solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight );
		//				solver.AddSysElement(rowIdx, v1             ,	-_S*weight);
		//				solver.AddSysElement(rowIdx, vertexNum + v1 ,	-_R*weight );
		//				solver.AddSysElement(rowIdx, v2             ,	 _S*weight );
		//				solver.AddSysElement(rowIdx, vertexNum + v2 ,	 _R*weight );
		//				*/
		//				rowIdx++;
		//				G[rowIdx][vertexNum + vList[k]] = weight;
		//				G[rowIdx][vertexNum + vList[k+1]] = -weight;
		//				/*
		//				solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight );
		//				solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight );
		//				solver.AddSysElement(rowIdx, v1                         ,	 _R*weight );
		//				solver.AddSysElement(rowIdx, vertexNum + v1             ,	-_S*weight );
		//				solver.AddSysElement(rowIdx, v2                         ,	-_R*weight );
		//				solver.AddSysElement(rowIdx, vertexNum + v2             ,	 _S*weight );
		//				*/
		//				rowIdx++;

		//			}

		//			double resize_ratio_x = Image_width*0.5 / referenceList[rbIdx].x;
		//			double resize_ratio_y = Image_height*1 / referenceList[rbIdx].y;

		//			float weight2 = 1.0 *  (1-patch[i].saliency)  /*(1-faceflexibleList[face_index])*/;

		//			double dlx = (referenceList[v1].x  - referenceList[v2].x)*resize_ratio_x;
		//			double dly = (referenceList[v1].y  - referenceList[v2].y)*resize_ratio_y;
		//			double centerl_delta = -dlx * dlx - dly * dly; 
		//			double lSR[2][2];
		//			lSR[0][0] = -dlx / (centerl_delta+0.00001);
		//			lSR[0][1] = -dly / (centerl_delta+0.00001);
		//			lSR[1][0] = -dly / (centerl_delta+0.00001);
		//			lSR[1][1] =  dlx / (centerl_delta+0.00001);

		//			for(int k = 0 ; k < 4 ; k++)
		//			{
		//				vector2 ltemp_edge;
		//				double x = resize_ratio_x*((double)(referenceList[vList[k]].x - referenceList[vList[k+1]].x));
		//				double y = resize_ratio_y*((double)(referenceList[vList[k]].y - referenceList[vList[k+1]].y));
		//				double __S =  (lSR[0][0] * x + lSR[0][1] * y) / (patch_quad_size);
		//				double __R =  (lSR[1][0] * x + lSR[1][1] * y) / (patch_quad_size);

		//				G[rowIdx][vList[k]] = weight2;
		//				G[rowIdx][vList[k+1]] = -weight2;
		//				/*
		//				G[rowIdx][v1] = -__S*weight2;
		//				G[rowIdx][vertexNum + v1] = -__R*weight2;
		//				G[rowIdx][v2] = __S*weight2;
		//				G[rowIdx][vertexNum + v2] = __R*weight2;
		//				*/
		//				//solver.AddSysElement(rowIdx, vList[k]       ,	 weight2 );
		//				//solver.AddSysElement(rowIdx, vList[k+1]     ,	-weight2 );
		//				//solver.AddSysElement(rowIdx, v1             ,	-__S*weight2);
		//				//solver.AddSysElement(rowIdx, vertexNum + v1 ,	-__R*weight2 );
		//				//solver.AddSysElement(rowIdx, v2             ,	 __S*weight2 );
		//				//solver.AddSysElement(rowIdx, vertexNum + v2 ,	 __R*weight2 );

		//				rowIdx++;

		//				G[rowIdx][vertexNum + vList[k]] = weight2;
		//				G[rowIdx][vertexNum + vList[k+1]] = -weight2;
		//				/*
		//				G[rowIdx][v1] = __R*weight2;
		//				G[rowIdx][vertexNum + v1] = -__S*weight2;
		//				G[rowIdx][v2] = -__R*weight2;
		//				G[rowIdx][vertexNum + v2] = __S*weight2;
		//				*/
		//				//solver.AddSysElement(rowIdx, vertexNum + vList[k]       ,	 weight2 );
		//				//solver.AddSysElement(rowIdx, vertexNum + vList[k+1]     ,	-weight2 );
		//				//solver.AddSysElement(rowIdx, v1                         ,	 __R*weight2 );
		//				//solver.AddSysElement(rowIdx, vertexNum + v1             ,	-__S*weight2 );
		//				//solver.AddSysElement(rowIdx, v2                         ,	-__R*weight2 );
		//				//solver.AddSysElement(rowIdx, vertexNum + v2             ,	 __S*weight2 );

		//				rowIdx++;

		//			}


		//		}
		//		center_edge_index++;
		//		//cout << endl;

		//	} else {
		//	}


		//}



		for(unsigned int i = 0 ; i < faceNum ; ++i)
		{
			int vList[4] = {faceList[i].x,faceList[i].y,faceList[i].z,faceList[i].w};

			G[rowIdx][vList[0]]   = edgeweight;
			G[rowIdx++][vList[3]] = -edgeweight;
			G[rowIdx][vList[1]]   = edgeweight;
			G[rowIdx++][vList[2]] = -edgeweight;
			G[rowIdx][vertexNum + vList[0]]   = edgeweight;
			G[rowIdx++][vertexNum + vList[1]] = -edgeweight;
			G[rowIdx][vertexNum + vList[2]]   = edgeweight;
			G[rowIdx++][vertexNum + vList[3]] = -edgeweight;

			/*
			solver.AddSysElement(rowIdx  , vList[0],     edgeweight);
			solver.AddSysElement(rowIdx++, vList[3],	-edgeweight);
			solver.AddSysElement(rowIdx  , vList[1],     edgeweight);
			solver.AddSysElement(rowIdx++, vList[2],	-edgeweight);
			solver.AddSysElement(rowIdx  , vertexNum + vList[0],     edgeweight);
			solver.AddSysElement(rowIdx++, vertexNum + vList[1],	-edgeweight);
			solver.AddSysElement(rowIdx  , vertexNum + vList[2],     edgeweight);
			solver.AddSysElement(rowIdx++, vertexNum + vList[3],	-edgeweight);
			*/
		}


		//Eigen::MatrixXd HMatrix;
		// transpose G

		double **tmpG = mallocArray<double>(rows,cols);
		double **tmpGT = mallocArray<double>(cols,rows);
		double **tmpH = mallocArray<double>(cols,cols);

		/*
		double *tmpG = (double *)malloc(rows*cols*sizeof(double));
		double *tmpGT = (double *)malloc(cols*rows*sizeof(double));
		double *tmpH = (double *)malloc(rows*rows*sizeof(double));
		*/
		for(int i = 0 ; i < rows; i++) {
			for(int j = 0 ; j < cols; j++) {
				GT[j][i] = G[i][j];
				tmpG[i][j] = G[i][j];
				tmpGT[j][i] = GT[j][i];
				if(i == 0 && j==0) {
					cout << tmpG[i][j] << endl;
				}
			}
		}
		for(int i = 0 ; i < cols; i++) {
			for(int j = 0 ; j < rows; j++) {
				tmpGT[i][j] = GT[i][j];
				if(i == 0 && j==0) {
					cout << tmpGT[i][j] << endl;
				}
			}

		}
		for(int i = 0 ; i < cols; i++) {
			for(int j = 0 ; j < cols; j++) {
				tmpH[i][j] = 0;
			}
		}
		std::cout << cols << "," << rows << endl;
		double time1  = clock() ;

		cudaSolver gSolver ;
		gSolver.Init() ;
		gSolver.tt2();
		gSolver.MatMul2(tmpGT,tmpG,tmpH,cols,rows);
		// G^T*G = H
		//#pragma omp parallel num_threads(8)
		/*
		{
		//#pragma omp for
		{
		for(int i = 0 ; i < cols; i++) {
		for(int j = 0 ; j < cols; j++) {
		double sum = 0;
		//#pragma omp for reduction( +:sum)
		{
		for(int k = 0 ; k < rows ; k++) {
		sum += GT[i][k] * G[k][j];
		//	sum += G[i][k] * GT[k][j];

		}
		}

		H[i][j] = sum;
		}

		}
		}
		}
		*/
		for(int i = 0 ; i < cols; i++) {
			for(int j = 0 ; j < cols; j++) {
				//	cout << H[i][j] << " " << tmpH[i][j] << endl;

				H[i][j] = tmpH[i][j];
				/*
				if(H[i][j] != tmpH[i][j]) {
				cout << H[i][j] << " " << tmpH[i][j] << " false " << endl;

				} else {
				cout << H[i][j] << " " << tmpH[i][j] << " true " << endl;
				}
				*/
			}
		}


		printf("time:%f\n",(clock()-time1)/CLK_TCK) ;
		cout << "multiple" << endl;

	}




	// ------------------------------------ set right hand size matrix --------------------------------
	rowIdx = 0;

	// sovle quadprog
	double time1  = clock() ;
	cout <<" result : "<< solve_quadprog(H, g0, CE, ce0, CI, ci0, xx) << endl;
	printf("time:%f\n",(clock()-time1)/CLK_TCK) ;
	// ------------------------------------------ initial guess ---------------------------------------
	rowIdx = 0;
	// ---------------------------------------------- solve -------------------------------------------
	/*
	solver.solve();

	*/
	backupVertex.clear();
	for(int i = 0 ; i < vertexNum ; i++)
	{
		vector2 pos(xx[rowIdx] , xx[vertexNum + rowIdx]);

		//	cout << pos.x << "," << pos.y << endl;
		//printf("(%f %f) => (%f %f)\n",referenceList[i].x,referenceList[i].y,pos.x,pos.y);
		backupVertex.push_back(pos);

		rowIdx++;
	}
	foldOver(backupVertex);

	/*
	G.~Matrix();
	GT.~Matrix();
	H.~Matrix();
	CE.~Matrix();
	CI.~Matrix();

	g0.~VectorQuad();
	ce0.~VectorQuad(); 
	ci0.~VectorQuad(); 
	xx.~VectorQuad(); 
	*/
}

void manager_image::foldOver(vector<vector2> backupVertex) {
	bool flowover = false ;
	for(int i = 0 ; i < faceList.size() ; i++)
	{
		vector2 a_vec =  backupVertex[faceList[i].y] - backupVertex[faceList[i].x] ;
		vector2 b_vec =  backupVertex[faceList[i].w] - backupVertex[faceList[i].x] ;
		if( a_vec.x*b_vec.y - a_vec.y*b_vec.x < 0 ) {
			flowover = true ;
			break;
		}
		a_vec =  backupVertex[faceList[i].w] - backupVertex[faceList[i].z] ;
		b_vec =  backupVertex[faceList[i].y] - backupVertex[faceList[i].z] ;
		if( a_vec.x*b_vec.y - a_vec.y*b_vec.x < 0 ) {
			flowover = true ;
			break;
		}
	}
	if(flowover) {
		cout << "fold over error" << endl;
	}
}

void manager_image::saveSelectedMeshFile(string filename) {
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

void manager_image::loadSelectedMeshFile(string filename) {
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
