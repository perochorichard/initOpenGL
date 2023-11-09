#include "Mesh.h"
#include "Shader.h"

Mesh::Mesh() {
	m_shader = nullptr;
	m_texture = { };
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_position = { 0, 0, 0 };
	m_rotation = { 0, 0, 0 };
}

Mesh::~Mesh() {

}

void Mesh::Create(Shader* _shader) {
	m_shader = _shader;
	
	m_texture = Texture();
	m_texture.LoadTexture("../Assets/Textures/Wood.jpg");

	m_vertexData = {
		/*   position   */     /* RGBA color */     /* texture coords */
		50.0f, 50.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, // top-right
		50.0f, -50.0f, 0.0f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // bottom-right
		-50.0f, -50.0f, 0.0f,  0.0f, 0.0f, 1.0f,    0.0f, 0.0f, // bottom-left
		-50.0f, 50.0f, 0.0f,   1.0f, 1.0f, 1.0f,    0.0f, 1.0f  //top-left
	};
	
	// creates a new buffer object and assigns unique ID to m_vertexBuffer for reference.
	glGenBuffers(1, &m_vertexBuffer); 
	// binds buffer object (using id) to the GL_ARRAY_BUFFER target.
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	// Allocate memory and copy the vertex data into the GL_ARRAY_BUFFER target.
	// The data will be used for rendering and will not change frequently (GL_STATIC_DRAW).
	glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(float), m_vertexData.data(), GL_STATIC_DRAW);

	m_indexData = {
		2, 0, 3, 2, 1, 0
	};

	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexData.size() * sizeof(GLubyte), m_indexData.data(), GL_STATIC_DRAW);
}

void Mesh::Cleanup() {
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_indexBuffer);
	m_texture.Cleanup();
}

void Mesh::Render(glm::mat4 _wvp) {
	glUseProgram(m_shader->GetProgramID()); // Use our shader

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(m_shader->GetAttrVertices()); // vertext position data will be used in the rendering process
	glVertexAttribPointer(m_shader->GetAttrVertices(), 
		3,                 // size 
		GL_FLOAT,          // type 
		GL_FALSE,          // normalized 
		8 * sizeof(float), // stride (8 floats per vertex definition)
		(void*)0);         // array buffer offset

	// end attribute buffer : colors
	glEnableVertexAttribArray(m_shader->GetAttrColors());
	glVertexAttribPointer(m_shader->GetAttrColors(),
		3,                 //size (3 vertices per primitive)
		GL_FLOAT,          // type
		GL_FALSE,          // normalized
		8 * sizeof(float), // stride
		(void*)(3 * sizeof(float))); // array buffer offset

	// 4th attribute : WVP
	m_rotation.y += 0.005f;
	glm::mat4 transform = glm::rotate(_wvp, m_rotation.y, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(m_shader->GetAttrWVP(), 1, GL_FALSE, &transform[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer); // bind the vertex buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer); // bind the index buffer
	glBindTexture(GL_TEXTURE_2D, m_texture.GetTexture()); // bind the texture
	glDrawElements(GL_TRIANGLES, m_indexData.size(), GL_UNSIGNED_BYTE, (void*)0);
	glDisableVertexAttribArray(m_shader->GetAttrColors());
	glDisableVertexAttribArray(m_shader->GetAttrVertices());
	glDisableVertexAttribArray(m_shader->GetAttrTexCoords());
}
