
attribute highp vec4 posAttr;
attribute lowp vec4 colAttr;
varying lowp vec4 col;
uniform highp mat4 worldMatrix;
uniform highp mat4 sphereMatrix;

void main()
{
	col = colAttr;
	gl_Position = worldMatrix * (sphereMatrix * posAttr);
}
