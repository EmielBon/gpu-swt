//
//  types.h
//  OGLTest
//
//  Created by Emiel Bon on 31-01-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

// Standard libraries
#include <string>
#include <sstream>
#include <memory>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <map>
#include <tuple>
#include <cfloat>
#include <stack>
#include <tuple>
#include <set>
#include <list>

// Third party libraries
#include <GL/glew.h>
#include <opencv2/core/core.hpp>

enum class GradientDirection
{
    With,
    Against,
};

// Typedefs
using String = std::string;
using StringStream = std::stringstream;
using Exception = std::exception;
using InputFileStream = std::ifstream;
using Vector2 = cv::Vec2f;
using Vector3 = cv::Vec3f;
using Point = cv::Vec2i;

template<class T> using Ptr = std::shared_ptr<T>;
template<class T> using Set = std::set<T>;
template<class T> using OrderedSet = std::set<T, bool(*)(T, T)>;
template<class T> using List = std::vector<T>;
template<class T> using LinkedList = std::list<T>;
template<class T> using Stack = std::stack<T>;
template<class T, class U> using Map = std::map<T, U>;

// Macros
#define New std::make_shared
#define Tuple std::tuple

// Forward declarations
// Framework
class BoundingBox;
class DrawableRect;
class Image;
class Window;
// Grahpics
class DepthBuffer;
class FragmentShader;
class FrameBuffer;
class GraphicsDevice;
class IndexBuffer;
class Program;
class Shader;
class Texture;
class Uniform;
class VertexArray;
class VertexBuffer;
class VertexDeclaration;
class VertexDeclarationElement;
class VertexShader;
// SWT specific
class  Chain;
struct Component;
struct Ray;