#include "Shader.h"

Shader::Shader() {
	m_programID = 0;
	m_attrVertices = 0;
	m_attrColors = 0;
	m_attrTexCoords = 0;
	m_attrWVP = 0;
	m_sampler1 = 0;
	m_sampler2 = 0;
	m_result = GL_FALSE;
	m_infoLogLength = 0;
}


void Shader::Cleanup() {
	glDeleteProgram(m_programID);
}

void Shader::LoadAttributes() {
	m_attrVertices = glGetAttribLocation(m_programID, "vertices"); // Get a handle for the vertex buffer
	m_attrColors = glGetAttribLocation(m_programID, "colors"); // handle for colors buffer
	m_attrTexCoords = glGetAttribLocation(m_programID, "texCoords"); // handle for texCoords buffer
	m_attrWVP = glGetUniformLocation(m_programID, "WVP"); // handle for WVP matrix
	m_sampler1 = glGetUniformLocation(m_programID, "sampler1"); // handle for texture sampler 1
	m_sampler2 = glGetUniformLocation(m_programID, "sampler2"); // handle for texture sampler 2
}

void Shader::EvaluateShader(int _infoLength, GLuint _id) {
	if (_infoLength > 0) {
		std::vector<char> errorMessage(_infoLength + 1);
		glGetShaderInfoLog(_id, _infoLength, NULL, &errorMessage[0]);
		M_ASSERT(0, ("%s\n", &errorMessage[0]));
	}
}

GLuint Shader::LoadShaderFile(const char* _filePath, GLenum _type) {
	GLuint shaderID = glCreateShader(_type); // create the shader

	// Read the Shader code from the file
	std::string shaderCode;
	std::ifstream shaderStream(_filePath, std::ios::in);
	M_ASSERT(shaderStream.is_open(), ("impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", _filePath));
	std::string Line = "";
	while (getline(shaderStream, Line))
		shaderCode += "\n" + Line;
	shaderStream.close();

	// Compile Shader
	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);

	// Check Shader
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &m_result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &m_infoLogLength);
	EvaluateShader(m_infoLogLength, shaderID);

	// Attach shader to program
	glAttachShader(m_programID, shaderID);

	return shaderID;
}

void Shader::CreateShaderProgram(const char* _vertexFilePath, const char* _fragmentFilePath) {
	m_programID = glCreateProgram(); // Create the shader program
	GLuint vertexShaderID = LoadShaderFile("SimpleVertexShader.vertexshader", GL_VERTEX_SHADER);
	GLuint fragmentShaderID = LoadShaderFile("SimpleFragmentShader.fragmentshader", GL_FRAGMENT_SHADER);

	if (CheckShaderCompilationStatus(vertexShaderID)) {
		std::cout << "Vertex shader compiled successfully..." << std::endl;
	}

	if (CheckShaderCompilationStatus(fragmentShaderID)) {
		std::cout << "Fragment shader compiled successfully..." << std::endl;
	}


	glLinkProgram(m_programID); // Link the program

	// check program
	glGetProgramiv(m_programID, GL_LINK_STATUS, &m_result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &m_infoLogLength);
	EvaluateShader(m_infoLogLength, m_programID);

	// Free resources
	glDetachShader(m_programID, vertexShaderID);
	glDetachShader(m_programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

void Shader::LoadShaders(const char* _vertexFilePath, const char* _fragmentFilePath) {
	CreateShaderProgram(_vertexFilePath, _fragmentFilePath);
	LoadAttributes();
}

bool Shader::CheckShaderCompilationStatus(GLuint shaderID) {
	GLint result = GL_FALSE;
	int infoLogLength;

	// Check shader compilation status
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 1) {
		std::vector<char> errorMessage(infoLogLength + 1);
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, &errorMessage[0]);
		M_ASSERT(0, ("Shader compilation error:\n%s\n", &errorMessage[0]));
		return false;
	}

	return true;
}
