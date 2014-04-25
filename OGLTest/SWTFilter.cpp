//
//  SWTFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 25-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "SWTFilter.h"
#include "VertexPositionTexture.h"
#include "Texture.h"
#include "GraphicsDevice.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

Ptr<Texture> SWTFilter::CastRays(const Texture &gradients, bool darkOnLight)
{
    glClearStencil(0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_STENCIL_TEST);
    
    cast->Use();
    //cast->Uniforms["Edges"].SetValue(edges);
    cast->Uniforms["Gradients"].SetValue(gradients);
    cast->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    auto values = Render("Stroke Width values");
    
    glDisable(GL_STENCIL_TEST);
    
    return values;
}

Ptr<Texture> SWTFilter::WriteRayValues(const Texture &values, const Texture &gradients, const Texture &lineLengths, bool darkOnLight)
{
    auto quadVertices = GraphicsDevice::VertexBuffer;
    auto quadIndices  = GraphicsDevice::IndexBuffer;
    
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    GraphicsDevice::SetBuffers(linesVertices, linesIndices);
    
    write->Use();
    // todo: need not pass x,y gradients, only direction matters
    write->Uniforms["Gradients"].SetValue(gradients);
    write->Uniforms["LineLengths"].SetValue(lineLengths);
    write->Uniforms["Values"].SetValue(values);
    write->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    auto strokeWidthTransform = Render((String("Stroke Width Transform (") + (darkOnLight ? "with" : "against") + " the gradient)").c_str());
    
    GraphicsDevice::SetBuffers(quadVertices, quadIndices);
    
    glDisable(GL_DEPTH_TEST);
    
    return strokeWidthTransform;
}

Ptr<Texture> SWTFilter::AverageRayValues(const Texture &values, const Texture &gradients, bool darkOnLight)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 2, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    avg->Use();
    avg->Uniforms["Gradients"].SetValue(gradients);
    avg->Uniforms["LineLengths"].SetValue(values);
    avg->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    auto averageValues = Render("Average Stroke Width values");
    
    glDisable(GL_STENCIL_TEST);
    
    return averageValues;
}

void SWTFilter::PrepareStencilTest()
{
    glClearStencil(0);
    glStencilFunc(GL_EQUAL, 2, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void SWTFilter::PrepareDepthTest()
{
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glClearDepth(1.0f);
}

void SWTFilter::PrepareRayLines(const Texture &values)
{
    linesVertices = New<::VertexBuffer>();
    linesIndices  = New<::IndexBuffer>(PrimitiveType::Lines);
    
    List<VertexPositionTexture> vertices;
    GLfloat buffer[values.GetWidth() * values.GetHeight()];
    values.GetTextureImage(GL_RED, GL_FLOAT, buffer);
    
    for(int i = 0; i < values.GetWidth(); ++i)
    for(int j = 0; j < values.GetHeight(); ++j)
    {
        if (buffer[i + j * values.GetWidth()] != 0.0f)
        {
            VertexPositionTexture v1, v2;
            v1.Position = Vector3(i, j, 0); // z == 0 = Use directly
            v2.Position = Vector3(i, j, 1); // z == 1 = Scatter position to end point
            vertices.push_back(v1);
            vertices.push_back(v2);
        }
    }
    
    // todo: skip index buffer for line drawing
    List<GLuint> indices( vertices.size() );
    GLuint counter = 0;
    for(auto& index : indices)
        index = counter++;
    
    linesVertices->SetData(vertices);
    linesIndices->SetData(indices);
}

Ptr<Texture> SWTFilter::PerformSteps(const Texture &input)
{
    PrepareDepthTest();
    PrepareStencilTest();
    
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    auto gradients = ApplyFilter(sobel, input);
    
    glEnable(GL_STENCIL_TEST);
    auto edges = ApplyFilter(canny, input);
    glDisable(GL_STENCIL_TEST);
    
    auto swt1 = CastRays(*gradients, true);
    auto swt2 = CastRays(*gradients, false);
    
    /*auto quadVertexBuffer = GraphicsDevice::VertexBuffer;
    auto quadIndexBuffer = GraphicsDevice::IndexBuffer;
    
    auto linesVertexBuffer = New<::VertexBuffer>();
    auto linesIndexBuffer = New<::IndexBuffer>(PrimitiveType::Lines);
    
    Ptr<Texture> strokeWidthValues, strokeWidthTransform, avgStrokeWidthValues, avgStrokeWidthTransform;
    
    glClearStencil(0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 2, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    strokeWidthTransform1->Use();
    strokeWidthTransform1->Uniforms["Edges"].SetValue(edges);
    strokeWidthTransform1->Uniforms["Gradients"].SetValue(gradients);
    strokeWidthTransform1->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    strokeWidthValues = Render("Stroke Width values");
    
    glDisable(GL_STENCIL_TEST);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    List<VertexPositionTexture> vertices;
    GLfloat buffer[edges.GetWidth() * edges.GetHeight()];
    strokeWidthValues->GetTextureImage(GL_RED, GL_FLOAT, buffer);
    
    for(int i = 0; i < edges.GetWidth(); ++i)
        for(int j = 0; j < edges.GetHeight(); ++j)
        {
            if (buffer[i + j * edges.GetWidth()] != 0.0f)
            {
                VertexPositionTexture v1, v2;
                v1.Position = Vector3(i, j, 0); // z == 0 = Use directly
                v2.Position = Vector3(i, j, 1); // z == 1 = Scatter position to end point
                vertices.push_back(v1);
                vertices.push_back(v2);
            }
        }
    
    // todo: skip index buffer for line drawing
    List<GLuint> indices( vertices.size() );
    GLuint counter = 0;
    for(auto& index : indices)
        index = counter++;
    
    linesVertexBuffer->SetData(vertices);
    linesIndexBuffer->SetData(indices);
    
    GraphicsDevice::SetBuffers(linesVertexBuffer, linesIndexBuffer);
    
    strokeWidthTransform2->Use();
    // todo: need not pass x,y gradients, only direction matters
    strokeWidthTransform2->Uniforms["Gradients"].SetValue(gradients);
    strokeWidthTransform2->Uniforms["LineLengths"].SetValue(*strokeWidthValues);
    strokeWidthTransform2->Uniforms["Values"].SetValue(*strokeWidthValues);
    strokeWidthTransform2->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    strokeWidthTransform = Render((String("Stroke Width Transform (") + (darkOnLight ? "with" : "against") + " the gradient)").c_str());
    
    GraphicsDevice::SetBuffers(quadVertexBuffer, quadIndexBuffer);
    
    glDisable(GL_DEPTH_TEST);
    
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 2, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    strokeWidthTransform3->Use();
    strokeWidthTransform3->Uniforms["Gradients"].SetValue(gradients);
    strokeWidthTransform3->Uniforms["LineLengths"].SetValue(*strokeWidthTransform);
    strokeWidthTransform3->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    avgStrokeWidthValues = Render("Average Stroke Width values");
    
    glDisable(GL_STENCIL_TEST);
    
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    GraphicsDevice::SetBuffers(linesVertexBuffer, linesIndexBuffer);
    
    strokeWidthTransform2->Use();
    strokeWidthTransform2->Uniforms["Gradients"].SetValue(gradients);
    strokeWidthTransform2->Uniforms["LineLengths"].SetValue(*strokeWidthValues);
    strokeWidthTransform2->Uniforms["Values"].SetValue(*avgStrokeWidthValues);
    strokeWidthTransform2->Uniforms["DarkOnLight"].SetValue(direction == GradientDirection::With);
    avgStrokeWidthTransform = Render((String("Average Stroke Width Transform (") + (darkOnLight ? "with" : "against") + " the gradient)").c_str());
    
    glDisable(GL_DEPTH_TEST);
    
    GraphicsDevice::SetBuffers(quadVertexBuffer, quadIndexBuffer);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    scaleColor->Use();
    scaleColor->Uniforms["Texture"].SetValue(*avgStrokeWidthTransform);
    scaleColor->Uniforms["Scale"].SetValue(1.0f / 50);
    auto scaledStrokeWidths = Render("Average Stroke Widths (scaled)");
    
    return avgStrokeWidthTransform;*/
}