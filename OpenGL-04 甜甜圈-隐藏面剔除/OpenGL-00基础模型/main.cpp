//
//  main.cpp
//  OpenGLProject
//
//  Created by gaoxin on 2020/7/6.
//  Copyright © 2020 gaoxin. All rights reserved.
//

#include "GLTools.h"
/*
 #include<GLTools.h> GLTool.h头文件包含了大部分GLTool中类似C语言的独立函数
 */
#include <glut/glut.h>
/*
 在Mac系统下，'#include<glut/glut.h>'
 在windows上和Linux，我们使用freeglut的静态库版本并且要添加一个宏
 */

/*
 #include<GLSaderManager.h> 移入了GLTool着色器管理器（shaderMananger)类。
 没有着色器。我们就不能再OpenGL（核心框架）进行着色。着色器管理器不允许我们创建并管理着色器，还提供一组“存储着色器”，他们
 能够进行一些初步了解基本的渲染操作
 */

#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"


// 设置角色帧，作为相机
GLFrame cameraFrame;
GLFrame viewFrame;
// 使用GLFrustum类来设置透视投影
GLFrustum viewFrustum;
GLTriangleBatch torusBath;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager shaderManager;

// 标记：背面剔除、深度测试
int iCull = 1;
int iDepth = 1;


// 窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w,h代表像素

void ChangeSize(int w,int h) {
   // 1.防止h变为0
    if (h == 0) {
        h = 1;
    }
    
    // 2.设置窗口尺寸
    glViewport(0, 0, w, h);
    
    // 3.setPerspective函数的参数是一个从顶点方向看去的视场角度（用角度值表示）
    // 设置透视模式，初始化其透视矩阵
    viewFrustum.SetPerspective(35, float(w)/float(h), 1.0f, 100.0f);
    
    // 4.把透视矩阵加载到透视矩阵对堆栈中
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    // 5.初始化渲染管线
    transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
}

// 为程序作一次性设置
void SetupRC() {
    // 1.清除一个或一组特定的缓冲区
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    
    // 2.初始化着色器管理器
    shaderManager.InitializeStockShaders();
    
    // 3.将相机向后移动7个单元：肉眼到物体之间的距离
    viewFrame.MoveForward(10);
    
    // 4.创建一个甜甜圈
    // void gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint mumMinor);
    // 参数1：GLTriangleBatch 容器帮助类
    // 参数2：外边缘半径
    // 参数3：内边缘半径
    // 参数4、5：主半径和从半径的细分单元数量
    gltMakeTorus(torusBath, 1.0f, 0.3f, 52, 26);
    
    // 5.点的大小（方便点填充式，肉眼观察）
    glPointSize(4.0f);
}

// 开始渲染
void RenderScene(void) {
    // 1.清除窗口和深度缓冲区
    // 不清空颜色/深度缓冲区时，渲染会造成什么样的后果，残留数据
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 2.把摄像机矩阵亚压入模型矩阵中
    modelViewMatrix.PushMatrix(viewFrame);
    
    // 3.设置绘图颜色
    GLfloat vRed[] = {1.0f,0.0f,0.0f,1.0f};
    
    if (iCull) {
        glEnable(GL_CULL_FACE);
    }
    if (iDepth) {
        glEnable(GL_DEPTH_TEST);
    }
    // 4.
    // 使用平面着色器
    // 参数1：平面着色器
    // 参数2：模型视图投影矩阵
    // 参数3：颜色
//     shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(), vRed);
    
    // 使用默认光源着色器
    // 通过光源、阴影效果更体现立体效果
    // 参数1：GLT_SHADER_EDFAULT_LIGHT 默认光源着色器
    // 参数2：模型视图矩阵
    // 参数3：投影矩阵
    // 参数4：基本颜色值
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vRed);
    
    // M3DMatrix44f mm;
    // modelViewMatrix.GetMatrix(mm);
    
    // M3DMatrix44f pp;
    // projectionMatrix.GetMatrix(pp);
    
    // shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, mm, pp, vRed)
    
    // 5.绘制
    torusBath.Draw();
    
    // 6.出战 绘制完成恢复
    modelViewMatrix.PopMatrix();
    
    // 7.交换缓冲区
    glutSwapBuffers();
}
/*
 参数2：x 鼠标位置 x值
 参数3：y 鼠标位置 y值
 */
void SpecialKeys(int key, int x, int y) {
    // 1判断方向
    if (key == GLUT_KEY_UP) {
        // 根据方向调整观察者位置
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_DOWN) {
        viewFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_LEFT) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        viewFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    glutPostRedisplay();
}

int main(int argc,char* argv[]) {
    // 设置当前工作目录，针对MAC OS X
    gltSetWorkingDirectory(argv[0]);
    
    // 初始化GLUT库
    glutInit(&argc, argv);
    
    /*初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCTL分别指双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区*/
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    //GLUT窗口太小，标题窗口
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("Geometry Test Program");
    
    // 注册回调函数
    glutReshapeFunc(ChangeSize);
    
    glutDisplayFunc(RenderScene);
    
    glutSpecialFunc(SpecialKeys);
    
    // 驱动程序的初始化中没有出现任何问题
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        fprintf(stderr, "glew error:%s\n",glewGetErrorString(err));
        return 1;
    }
    
    // 调用SetupRC
    SetupRC();
    
    glutMainLoop();
    return 0;
}

