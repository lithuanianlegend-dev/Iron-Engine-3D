#version 460 core

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 FragColor;

uniform sampler2D u_Texture;

uniform float u_AmbientStrength;
uniform float u_SpecularStrength;

uniform vec3 u_LightColor;
uniform vec3 u_ObjectColor;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

void main()
{

	// Ambient
	vec3 ambient = u_AmbientStrength * u_LightColor;

	// Diffuse
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(u_LightPos - v_FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * u_LightColor;
	
	// Specular
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = u_SpecularStrength * spec * u_LightColor;

	vec3 result = (ambient + diffuse + specular) * u_ObjectColor * texture(u_Texture, v_TexCoord).rgb;
	FragColor = vec4(result, 1.0);
}