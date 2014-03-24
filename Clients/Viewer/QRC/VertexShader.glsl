
attribute highp vec4 verticesAttr;
attribute lowp vec4 colorsAttr;
varying lowp vec4 col;
uniform highp mat4 worldMatrix;
uniform highp mat4 sphereMatrix;

void main()
{
    col = colorsAttr;
    gl_Position = worldMatrix * (sphereMatrix * verticesAttr);
}
