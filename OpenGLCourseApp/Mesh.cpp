#include "Mesh.h"



Mesh::Mesh()
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

void Mesh::CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCount = numOfIndices;

	glGenVertexArrays(1, &VAO); // [1.1] Create VAO ID [1.1]
	glBindVertexArray(VAO); // [1.2] Bind the VAO with that ID [1.2]

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO); // [1.3] Create VBO ID inside the VAO [1.3]
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // [1.4] Bind VBA to the GLA_ARRAY_BUFFER [1.4]
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW); // [1.5] Bind the buffer data (vertices) to the GL_ARRAY_BUFFER VBO [1.5]

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // [1.6] Define the attribute pointer formatting [1.6]
	glEnableVertexAttribArray(0); // [1.7] Enable the attirube pointer [1.7]

	glBindBuffer(GL_ARRAY_BUFFER, 0); // [1.8.1] Unbind the VBO [1.8.1]
	glBindVertexArray(0); // [1.8.2] Unbind the VAO [1.8.2]
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // The IBO should alway be unbinded after the VAO. 
}

void Mesh::RenderMesh()
{
	glBindVertexArray(VAO); // Binding the VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // Binding the IBO
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0); // Draw the pyramid
	glBindVertexArray(0); // Unbinding the VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // Unbinding the IBO
}

void Mesh::ClearMesh()
{
	// Ensure the buffer and array objects are deleted off the graphics card
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO); 
		IBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO); 
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;
}

Mesh::~Mesh()
{
	ClearMesh();
}
