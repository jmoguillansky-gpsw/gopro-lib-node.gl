TMP_DIR=/tmp/nodegl/external
wget -nc https://github.com/Stupeflix/sxplayer/archive/v9.5.1.tar.gz  -P $TMP_DIR/sxplayer
wget -nc https://sourceforge.net/projects/pthreads4w/files/pthreads-w32-2-9-1-release.zip -P $TMP_DIR
wget -nc https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.bin.WIN64.zip -P $TMP_DIR
wget -nc https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip -P $TMP_DIR
wget -nc https://www.khronos.org/registry/OpenGL/api/GL/glext.h -P $TMP_DIR/gl/include/GL
wget -nc https://www.khronos.org/registry/OpenGL/api/GL/glcorearb.h -P $TMP_DIR/gl/include/GL
wget -nc https://www.khronos.org/registry/OpenGL/api/GL/wglext.h -P $TMP_DIR/gl/include/GL
wget -nc https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h -P $TMP_DIR/gl/include/KHR
tar xzf $TMP_DIR/sxplayer/v9.5.1.tar.gz
bash scripts/apply_patches.sh
mkdir win64
cd win64 && unzip $TMP_DIR/glfw-3.3.2.bin.WIN64.zip; cd -
cd win64 && unzip -d pthreads-w32-2-9-1-release $TMP_DIR/pthreads-w32-2-9-1-release.zip; cd -
mkdir -p win64/gl/include && cp -rf $TMP_DIR/gl/include/* win64/gl/include/.
cd win64 && unzip -d glew-2.1.0-win32 $TMP_DIR/glew-2.1.0-win32.zip; cd -
