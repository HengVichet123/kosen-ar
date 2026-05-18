
#define __GLMETASEQ_C__
#include <string.h>
#include <ctype.h>
#include "GLMetaseq.h"

/*

GLMetaseq
The MIT License
Copyright (c) 2009 Sunao Hashimoto and Keisuke Konishi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


GLMetaseq
MIT���C�Z���X
Copyright (c) 2009 Sunao Hashimoto and Keisuke Konishi

�ȉ��ɒ�߂�����ɏ]���A�{�\�t�g�E�F�A����ъ֘A�����̃t�@�C���i�ȉ��u�\�t�g
�E�F�A�v�j�̕������擾���邷�ׂĂ̐l�ɑ΂��A�\�t�g�E�F�A�𖳐����Ɉ������Ƃ�
�����ŋ����܂��B����ɂ́A�\�t�g�E�F�A�̕������g�p�A���ʁA�ύX�A�����A�f�ځA
�Еz�A�T�u���C�Z���X�A�����/�܂��͔̔����錠���A����у\�t�g�E�F�A��񋟂���
����ɓ������Ƃ������錠�����������Ɋ܂܂�܂��B 

��L�̒��쌠�\������і{�����\�����A�\�t�g�E�F�A�̂��ׂĂ̕����܂��͏d�v�ȕ���
�ɋL�ڂ�����̂Ƃ��܂��B 

�\�t�g�E�F�A�́u����̂܂܁v�ŁA�����ł��邩�Öقł��邩���킸�A����̕ۏ�
���Ȃ��񋟂���܂��B�����ł����ۏ؂Ƃ́A���i���A����̖ړI�ւ̓K�����A�����
������N�Q�ɂ��Ă̕ۏ؂��܂݂܂����A����Ɍ��肳�����̂ł͂���܂���B 
��҂܂��͒��쌠�҂́A�_��s�ׁA�s�@�s�ׁA�܂��͂���ȊO�ł��낤�ƁA�\�t�g
�E�F�A�ɋN���܂��͊֘A���A���邢�̓\�t�g�E�F�A�̎g�p�܂��͂��̑��̈�����
����Đ������؂̐����A���Q�A���̑��̋`���ɂ��ĉ���̐ӔC������Ȃ�����
�Ƃ��܂��B 

*/

/*=========================================================================
�y���̃\�[�X���ł̂ݗL���ȃO���[�o���ϐ��z
=========================================================================*/

static TEXTURE_POOL l_texPool[MAX_TEXTURE];		// �e�N�X�`���v�[��
static int			l_texPoolnum;				// �e�N�X�`���̐�
static int			l_GLMetaseqInitialized = 0;	// �������t���O


/*=========================================================================
�y�֐��錾�z
=========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

void	endianConverter(void *addr,unsigned int size);
void	TGAHeaderEndianConverter(	STR_TGA_HEAD *tgah );
int		IsExtensionSupported( char* szTargetExtension );

GLuint		mqoSetTexturePool(char *texfile, char *alpfile, unsigned char alpha );
void		mqoClearTexturePool();
GLubyte*	mqoLoadTextureEx(char *texfile,char *alpfile,int *tex_size,unsigned char alpha);
int			mqoLoadFile(MQO_OBJECT *mqoobj,char *filename,double scale,unsigned char alpha);
MQO_OBJECT*	mqoCreateList(int num);
int			mqoCreateListObject( MQO_OBJECT *obj, int id, char *filename,double scale,unsigned char alpha);

void mqoCallListObject(MQO_OBJECT object[],int num);
void mqoClearObject(MQO_OBJECT object[],int from,int num);
void mqoDeleteObject(MQO_OBJECT * object,int num);
void mqoGetDirectory(const char *path_file, char *path_dir);
void mqoSnormal(glPOINT3f A, glPOINT3f B, glPOINT3f C, glPOINT3f *normal);
void mqoReadMaterial(FILE *fp, MQO_MATDATA M[]);
void mqoReadVertex(FILE *fp, glPOINT3f V[]);
int	 mqoReadBVertex(FILE *fp,glPOINT3f V[]);
void mqoReadFace(FILE *fp, MQO_FACE F[]);
void mqoReadObject(FILE *fp, MQO_OBJDATA *obj);

void mqoMakeArray(MQO_MATERIAL *mat, int matpos,MQO_FACE F[], int fnum,glPOINT3f V[],
				  glPOINT3f N[], double facet, glCOLOR4f *mcol, double scale, unsigned char alpha );

glPOINT3f *mqoVertexNormal(MQO_OBJDATA *obj);

void mqoMakePolygon(MQO_OBJDATA *readObj, MQO_OBJECT *mqoobj,
					glPOINT3f N[], MQO_MATDATA M[], int n_mat, double scale, unsigned char alpha);

void mqoMakeObjectsEx(MQO_OBJECT *mqoobj, MQO_OBJDATA obj[], int n_obj, MQO_MATDATA M[],int n_mat,
					  double scale,unsigned char alpha);

#ifdef __cplusplus
}
#endif


/*=========================================================================
�y�֐��zendianConverter
�y�p�r�z�G���f�B�A���ϊ�
�y�����z
		addr	�A�h���X
		size	�T�C�Y

�y�ߒl�z�Ȃ�
=========================================================================*/

void endianConverter(void *addr,unsigned int size)
{
	unsigned int pos;
	char c;
	if ( size <= 1 ) return;
	for ( pos = 0; pos < size/2; pos++ ) {
		c = *(((char *)addr)+pos);
		*(((char *)addr)+pos) = *(((char *)addr)+(size-1 - pos));
		*(((char *)addr)+(size-1 - pos)) = c;
	}
}


/*=========================================================================
�y�֐��zTGAHeaderEndianConverter
�y�p�r�zTGA�̃w�b�_�̃G���f�B�A���ϊ�
�y�����z
		tgah	TGA�̃w�b�_

�y�ߒl�z�Ȃ�
=========================================================================*/

void TGAHeaderEndianConverter(	STR_TGA_HEAD *tgah )
{
	endianConverter(&tgah->color_map_entry,sizeof(tgah->color_map_entry));
	endianConverter(&tgah->x,sizeof(tgah->x));
	endianConverter(&tgah->y,sizeof(tgah->y));
	endianConverter(&tgah->width,sizeof(tgah->width));
	endianConverter(&tgah->height,sizeof(tgah->height));
}


/*=========================================================================
�y�֐��zIsExtensionSupported
�y�p�r�zOpenGL�̊g���@�\���T�|�[�g����Ă��邩�ǂ������ׂ�
�y�����z
		szTargetExtension	�g���@�\�̖��O

�y�ߒl�z1�F�T�|�[�g����Ă���C0�F����Ă��Ȃ�
=========================================================================*/

int IsExtensionSupported( char* szTargetExtension )
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;

	// Extension �̖��O�������������ׂ�(NULL��󔒂�NG�j
	pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
	if ( pszWhere || *szTargetExtension == (char)NULL )
		return 0;

	// Extension �̕��������������
	pszExtensions = glGetString( GL_EXTENSIONS );

	// ������̒��ɕK�v�� extension �����邩���ׂ�
	pszStart = pszExtensions;
	for (;;)
	{
		pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
		if ( !pszWhere )
			break;
		pszTerminator = pszWhere + strlen( szTargetExtension );
		if ( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
		if ( *pszTerminator == ' ' || *pszTerminator == (char)NULL )
			return 1;
		pszStart = pszTerminator;
	}
	return 0;
}


/*=========================================================================
�y�֐��zmqoInit
�y�p�r�z���^�Z�R�C�A���[�_�̏�����
�y�����z�Ȃ�
�y�ߒl�z�Ȃ�
=========================================================================*/

void mqoInit(void)
{
	// �e�N�X�`���v�[��������
	memset(l_texPool,0,sizeof(l_texPool));
	l_texPoolnum = 0;

	// ���_�o�b�t�@�̃T�|�[�g�̃`�F�b�N
	g_isVBOSupported = IsExtensionSupported("GL_ARB_vertex_buffer_object");
//	g_isVBOSupported = 0;

#ifdef WIN32
	glGenBuffersARB = NULL;
	glBindBufferARB = NULL;
	glBufferDataARB = NULL;
	glDeleteBuffersARB = NULL;

	if ( g_isVBOSupported ) {
		// printf("OpenGL : ���_�o�b�t�@���T�|�[�g���Ă���̂Ŏg�p���܂�\n");
		// GL �֐��̃|�C���^����������
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
	}
#endif

	// �������t���O
	l_GLMetaseqInitialized = 1;
}


/*=========================================================================
�y�֐��zmqoCleanup
�y�p�r�z���^�Z�R�C�A���[�_�̏I������
�y�����z�Ȃ�
�y�ߒl�z�Ȃ�
=========================================================================*/

void mqoCleanup(void)
{
	mqoClearTexturePool();	// �e�N�X�`���v�[���̃N���A
}


/*=========================================================================
�y�֐��zmqoSetTexturePool
�y�p�r�z�e�N�X�`���v�[���Ƀe�N�X�`����ǂݍ���
�y�����z
		texfile		�e�N�X�`���t�@�C����
		alpfile		�A���t�@�t�@�C����
		alpha		�A���t�@

�y�ߒl�z�e�N�X�`��ID
�y�d�l�z�e�N�X�`�����܂��ǂݍ��܂�Ă��Ȃ���Γǂݍ��݁C�e�N�X�`���o�^
		���łɓǂݍ��܂�Ă���Γo�^�������̂�Ԃ�.
=========================================================================*/

GLuint mqoSetTexturePool(char *texfile, char *alpfile, unsigned char alpha ) 
{
	int pos;
	GLubyte *image;

	for ( pos = 0; pos < l_texPoolnum; pos++ ) {
		if ( alpha != l_texPool[pos].alpha ) {
			continue;
		}
		if ( texfile != NULL ) {
			if ( strcmp(texfile,l_texPool[pos].texfile) != 0 ) {
				continue;
			}
		}
		if ( alpfile != NULL ) {
			if ( strcmp(alpfile,l_texPool[pos].alpfile) != 0 ) {
				continue;
			}
		}
		break;
	}
	if ( pos < l_texPoolnum ) { //���łɓǂݍ��ݍς�
		return  l_texPool[pos].texture_id;
	}
	if ( MAX_TEXTURE <= pos ) {
		printf("%s:mqoSetTexturePool �e�N�X�`���ǂݍ��ݗ̈�s��\n",__FILE__);
		return -1;
	}
	image = mqoLoadTextureEx(texfile,alpfile,&l_texPool[pos].texsize,alpha);
	if ( image == NULL ) {
		return -1;
	}

	if ( texfile != NULL ) strncpy(l_texPool[pos].texfile,texfile,MAX_PATH);
	if ( alpfile != NULL ) strncpy(l_texPool[pos].alpfile,alpfile,MAX_PATH);
	l_texPool[pos].alpha = alpha;

	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	glPixelStorei(GL_PACK_ALIGNMENT,4);
	glGenTextures(1,&l_texPool[pos].texture_id);			// �e�N�X�`���𐶐�
	glBindTexture(GL_TEXTURE_2D,l_texPool[pos].texture_id);	// �e�N�X�`���̊��蓖��

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, l_texPool[pos].texsize, l_texPool[pos].texsize,
					0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	l_texPoolnum = pos+1;

	//�o�^����΁A�ǂݍ��񂾃o�b�t�@�͕s�v
	free(image);
	glBindTexture(GL_TEXTURE_2D,0);	// �f�t�H���g�e�N�X�`���̊��蓖��

	return l_texPool[pos].texture_id;
}


/*=========================================================================
�y�֐��zmqoClearTexturePool()
�y�p�r�z�e�N�X�`���v�[���̊J��
�y�����z�Ȃ�
�y�ߒl�z�Ȃ�
=========================================================================*/

void mqoClearTexturePool()
{
	int pos;
	for ( pos = 0; pos < l_texPoolnum; pos++ ) {
		glDeleteTextures(1, &l_texPool[pos].texture_id);	// �e�N�X�`�������폜
	}

	memset(l_texPool,0,sizeof(l_texPool));
	l_texPoolnum = 0;
}


/*=========================================================================
�y�֐��zmqoLoadTextureEx
�y�p�r�z�t�@�C������e�N�X�`���摜���쐬����
�y�����z
		texfile		�t�@�C����
		alpfile		�A���t�@�t�@�C����
		tex_size	�e�N�X�`���̃T�C�Y�i��ӂ̒����j��Ԃ�

�y�ߒl�z�e�N�X�`���摜�ւ̃|�C���^�i���s����NULL�j
�y�d�l�z24bit�r�b�g�}�b�v�C�����8,24,32bit�s�f�`
		�T�C�Y�́u��ӂ�2��n��̐����`�v�Ɍ���
		libjpeg,libpng�i�O�����C�u�����j���L���JPEG,PNG�̓ǂݍ��݉\
=========================================================================*/

GLubyte* mqoLoadTextureEx(char *texfile,char *alpfile,int *tex_size,unsigned char alpha)
{
	FILE *fp;
	size_t namelen;
	char ext[4];
	char wbuf[3];
	int isTGA;
	int isPNG;
	int isJPEG;
	int other;
	int	y,x,size;
	int fl;
	char *filename[2];
	int width[2];
	int sts;
	STR_TGA_HEAD tgah;
	GLubyte	*pImage, *pRead;

#if DEF_USE_LIBJPEG
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY jpegimage;
#endif
#if DEF_USE_LIBPNG
	unsigned char **pngimage;
	unsigned long   pngwidth, pngheight;
	int				pngdepth;
	int             color_type;
#endif

	filename[0] = texfile;
	filename[1] = alpfile;
	width[0] = -1;
	width[1] = -1;
	pImage = NULL;
	fp = NULL;
	sts = 0;
#if DEF_USE_LIBJPEG
	jpegimage = NULL;
#endif
#if DEF_USE_LIBPNG
	pngimage = NULL;
#endif
	size = - 1;
	for ( fl = 0; fl < 2; fl++ ) {//�e�N�X�`����fl=0    �A���t�@��fl=1
		if ( filename[fl] == NULL ) continue;
		namelen = strlen(filename[fl]);
		ext[0] = tolower(filename[fl][namelen-3]);
		ext[1] = tolower(filename[fl][namelen-2]);
		ext[2] = tolower(filename[fl][namelen-1]);
		ext[3] = 0x00;
		isTGA = (strcmp(ext,"tga")==0)?1:0;
		isPNG = (strcmp(ext,"png")==0)?1:0;
		isJPEG = (strcmp(ext,"jpg")==0)?1:0;
		/* */
		if ( (! isTGA) && (! isPNG) &&(! isJPEG) ) {
			filename[fl][namelen-3] = 'b';
			filename[fl][namelen-2] = 'm';
			filename[fl][namelen-1] = 'p';
		}
		/* */
		if ( fl == 1 ) { //�A���t�@�̓ǂݍ��݂͂s�f�`or�o�m�f
			if ( ! (isTGA || isPNG) ) {
				printf("�A���t�@�̃t�@�C���ɑΉ��ł��Ȃ���%s\n",filename[fl]);
				break;
			}
		}
		if ( fp != NULL ) fclose(fp);
		if ( (fp=fopen(filename[fl],"rb"))==NULL ) {
			printf("%s:�e�N�X�`���ǂݍ��݃G���[[%s]\n",__FILE__,filename[fl]);
			continue;
		}
		// �w�b�_�̃��[�h
		if ( isTGA ) {
			fread(&tgah,sizeof(STR_TGA_HEAD),1,fp);
#if DEF_IS_LITTLE_ENDIAN
#else
			TGAHeaderEndianConverter(&tgah);
#endif
			size = width[fl] = tgah.width;
		}
		if ( isJPEG ) {
#if DEF_USE_LIBJPEG
			unsigned int i;
			cinfo.err = jpeg_std_error( &jerr );
			jpeg_create_decompress( &cinfo );	//�𓀗p���쐬
			jpeg_stdio_src( &cinfo, fp );		//�ǂݍ��݃t�@�C���w��
			jpeg_read_header( &cinfo, TRUE );	//jpeg�w�b�_�ǂݍ���
			jpeg_start_decompress( &cinfo );	//�𓀊J�n

			if ( cinfo.out_color_components == 3 && cinfo.out_color_space == JCS_RGB ) {
				if ( jpegimage != NULL ) {
					for (i = 0; i < cinfo.output_height; i++) free(jpegimage[i]);            // �ȉ��Q�s�͂Q�����z���������܂�
					free(jpegimage);
				}
				//�ǂݍ��݃f�[�^�z��̍쐬
				jpegimage = (JSAMPARRAY)malloc( sizeof( JSAMPROW ) * cinfo.output_height );
				for ( i = 0; i < cinfo.output_height; i++ ) {
					jpegimage[i] = (JSAMPROW)malloc( sizeof( JSAMPLE ) * cinfo.out_color_components * cinfo.output_width );
				}
				//�𓀃f�[�^�ǂݍ���
				while( cinfo.output_scanline < cinfo.output_height ) {
					jpeg_read_scanlines( &cinfo,
						jpegimage + cinfo.output_scanline,
						cinfo.output_height - cinfo.output_scanline
					);
				}
				size = width[fl] = cinfo.output_width;
			}

			jpeg_finish_decompress( &cinfo );	//�𓀏I��
			jpeg_destroy_decompress( &cinfo );	//�𓀗p�����
			if ( !(cinfo.out_color_components == 3 && cinfo.out_color_space == JCS_RGB) ) {
				printf("JPEG �Ή��ł��Ȃ��t�H�[�}�b�g��%s\n",filename[fl]);
			}
#else
			printf("���̃e�N�X�`���͑Ή��ł��Ȃ��t�H�[�}�b�g��%s\n",filename[fl]);
			continue;
#endif
		}
		if ( isPNG ) {
#if DEF_USE_LIBPNG
			png_structp     png_ptr;
			png_infop       info_ptr;
			int             bit_depth, interlace_type;
			unsigned int             i;
			int j,k;
			png_ptr = png_create_read_struct(                       // png_ptr�\���̂��m�ہE���������܂�
							PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			info_ptr = png_create_info_struct(png_ptr);             // info_ptr�\���̂��m�ہE���������܂�
			png_init_io(png_ptr, fp);                               // libpng��fp��m�点�܂�
			png_read_info(png_ptr, info_ptr);                       // PNG�t�@�C���̃w�b�_��ǂݍ��݂܂�
			png_get_IHDR(png_ptr, info_ptr, &pngwidth, &pngheight,        // IHDR�`�����N�����擾���܂�
							&bit_depth, &color_type, &interlace_type,
							&j,&k);
			if ( pngimage != NULL ) {
				for (i = 0; i < pngheight; i++) free(pngimage[i]);            // �ȉ��Q�s�͂Q�����z���������܂�
				free(pngimage);
			}
			pngimage = (png_bytepp)malloc(pngheight * sizeof(png_bytep)); // �ȉ��R�s�͂Q�����z����m�ۂ��܂�
			i = png_get_rowbytes(png_ptr, info_ptr);
			pngdepth = i / pngwidth;
			for (i = 0; i < pngheight; i++)
					pngimage[i] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
			png_read_image(png_ptr, pngimage);                         // �摜�f�[�^��ǂݍ��݂܂�

			png_destroy_read_struct(                                // �Q�̍\���̂̃�������������܂�
	        &png_ptr, &info_ptr, (png_infopp)NULL);
			size = width[fl] = pngwidth;
#else
			printf("���̃e�N�X�`���͑Ή��ł��Ȃ��t�H�[�}�b�g��%s\n",filename[fl]);
			continue;
#endif
		}
		if ( width[fl] == -1 ) {//�R�R�܂ł��ăT�C�Y���w�肳��Ă��Ȃ��@���@�r�b�g�}�b�v
			fseek(fp,14+4,SEEK_SET);		// �摜�����i�[����Ă���ʒu�܂ŃV�[�N
			fread(&size,sizeof(int),1,fp);	// BiWidth�̏�񂾂��擾
			fseek(fp,14+40,SEEK_SET);		// ��f�f�[�^���i�[����Ă���ʒu�܂ŃV�[�N
#if DEF_IS_LITTLE_ENDIAN
#else
			endianConverter(&size,sizeof(int));
#endif
			width[fl] = size;

		}
		if ( width[0] != -1 && width[1] != -1 ) {
			if ( width[0] != width[1] ) {
				sts = -1;
				break;
			}
		}
		if ( fl == 1 && isTGA ) { //�A���t�@�̓ǂݍ��݂͂s�f�`�̂W�r�b�g���m�N�������R�Q�r�b�g�t��
			if ( !(
				(tgah.depth == 8 && tgah.type == DEF_TGA_TYPE_MONO) ||
				(tgah.depth == 32 && tgah.type == DEF_TGA_TYPE_FULL) 
				) ) {
				break;
			}
		}
		if ( fl == 1 && isPNG ) { //�A���t�@�̓ǂݍ��݂͂o�m�f�̃g�D���[�J���[�{�A���t�@�����O���[�X�P�[���{�A���t�@
#if DEF_USE_LIBPNG
			if ( !(
				(color_type== 6 ) ||
				(color_type== 4 ) 
				) ) {
				break;
			}
#endif
		}

		// �������̊m��
		if ( pImage == NULL ) {
			pImage = (GLubyte*)malloc(sizeof(unsigned char)*size*size*4);
		}
		if (pImage==NULL) return NULL;
		for (y=0; y<size; y++){
			pRead = pImage + (size-1-y)*4*size;
			for (x=0; x<size; x++) {
				other = 1;
				if ( fl == 0 ) {
					if ( isJPEG ) {
#if DEF_USE_LIBJPEG
						pRead[0]= jpegimage[size-1-y][x*3];
						pRead[1]= jpegimage[size-1-y][x*3+1];
						pRead[2]= jpegimage[size-1-y][x*3+2];
						pRead[3] = alpha;				// A
						other = 0;
#endif
					}
					if ( isPNG ) {
#if DEF_USE_LIBPNG
						if ( color_type == 2 || color_type==6 ) { 
							pRead[0]= pngimage[size-1-y][x*pngdepth];
							pRead[1]= pngimage[size-1-y][x*pngdepth+1];
							pRead[2]= pngimage[size-1-y][x*pngdepth+2];
							pRead[3] = alpha;				// A
							if ( color_type == 6 ) {
								pRead[3]= pngimage[size-1-y][x*pngdepth+3];
							}
						}
						other = 0;
#endif
					}
					if ( other )  {
						fread(&pRead[2],1,1,fp);	// B
						fread(&pRead[1],1,1,fp);	// G	
						fread(&pRead[0],1,1,fp);	// R
						pRead[3] = alpha;				// A
						if ( isTGA && tgah.depth == 32 ) {
							fread(&pRead[3],1,1,fp);	// A
							if ( alpha < pRead[3] ) pRead[3] = alpha;
						}
					}
				}
				else {
					if ( isPNG ) {
#if DEF_USE_LIBPNG
						if ( color_type == 6 ) { //�g�D���[�J���[�{�A���t�@
							pRead[3]= pngimage[size-1-y][x*pngdepth+3];
						}
						if ( color_type == 4 ) { //�O���[�X�P�[���{�A���t�@
							pRead[3]= pngimage[size-1-y][x*pngdepth+1];
						}
						if ( alpha < pRead[3] ) pRead[3] = alpha;
#endif
					}
					if ( isTGA ) {
						if ( tgah.depth == 32 ) { //����Ȃ��f�[�^��ǂݔ�΂�
							fread(wbuf,3,1,fp);	// BGR
						}
						fread(&pRead[3],1,1,fp);	// A
						if ( alpha < pRead[3] ) pRead[3] = alpha;
					}
				}
				pRead+=4;
			}
		}
		fclose(fp);
		fp = NULL;
	}
	if ( sts != 0 ) {
		if ( pImage != NULL ) free(pImage);
		if ( fp != NULL ) fclose(fp);
	}
#if DEF_USE_LIBPNG
	if ( pngimage != NULL ) {
		unsigned int uy;
		for (uy = 0; uy < pngheight; uy++) free(pngimage[uy]);            // �ȉ��Q�s�͂Q�����z���������܂�
		free(pngimage);
	}
#endif
#if DEF_USE_LIBJPEG
	if ( jpegimage != NULL ) {
		unsigned int uy;
		for (uy = 0; uy < cinfo.output_height; uy++) free(jpegimage[uy]);            // �ȉ��Q�s�͂Q�����z���������܂�
		free(jpegimage);
	}
#endif
	if ( size < 0 ) {
		if ( pImage != NULL ) free(pImage);
		pImage = NULL;
	}
	*tex_size = size;

	return pImage;
}


/*=========================================================================
�y�֐��zmqoLoadFile
�y�p�r�z���^�Z�R�C�A�t�@�C��(*.mqo)����f�[�^��ǂݍ���
�y�����z
		mqoobj		MQO�I�u�W�F�N�g
		filename	�t�@�C���̃p�X
		scale		�g�嗦
		alpha		�A���t�@

�y�ߒl�z�����F1 �^ ���s�F0
=========================================================================*/

int mqoLoadFile( MQO_OBJECT *mqoobj, char *filename, double scale, unsigned char alpha)
{
	FILE			*fp;
	MQO_OBJDATA		obj[MAX_OBJECT];
	MQO_MATDATA		*M = NULL;

	char	buf[SIZE_STR];		// ������ǂݍ��݃o�b�t�@
	char	path_dir[SIZE_STR];	// �f�B���N�g���̃p�X
	char	path_tex[SIZE_STR];	// �e�N�X�`���t�@�C���̃p�X
	char	path_alp[SIZE_STR];	// �A���t�@�e�N�X�`���t�@�C���̃p�X
	int		n_mat = 0;			// �}�e���A����
	int		n_obj = 0;			// �I�u�W�F�N�g��
	int		i;

	// Material��Object�̓ǂݍ���
	fp = fopen(filename,"rb");
	if (fp==NULL) return 0;

	mqoobj->alpha = alpha;
	memset(obj,0,sizeof(obj));

	i = 0;
	while ( !feof(fp) ) {
		fgets(buf,SIZE_STR,fp);

		// Material
		if (strstr(buf,"Material")) {
			sscanf(buf,"Material %d", &n_mat);
			M = (MQO_MATDATA*) calloc( n_mat, sizeof(MQO_MATDATA) );
			mqoReadMaterial(fp,M);
		}

		// Object
		if (strstr(buf,"Object")) {
			sscanf(buf,"Object %s", obj[i].objname);
			mqoReadObject(fp, &obj[i]);
			i++;
		}
	}
	n_obj = i;
	fclose(fp);

	// �p�X�̎擾
	mqoGetDirectory(filename, path_dir);

	// �e�N�X�`���̓o�^
	for (i=0; i<n_mat; i++) {
		if (M[i].useTex) {

			if (strstr(M[i].texFile,":")) {
				strcpy(path_tex, M[i].texFile);	// ��΃p�X�̏ꍇ
			} else {
				sprintf(path_tex,"%s%s",path_dir,M[i].texFile);	// ���΃p�X�̏ꍇ
			}

			if ( M[i].alpFile[0] != (char)0 ) {
				if (strstr(M[i].texFile,":")) {
					strcpy(path_alp, M[i].alpFile);	// ��΃p�X�̏ꍇ
				} else {
					sprintf(path_alp,"%s%s",path_dir,M[i].alpFile);	// ���΃p�X�̏ꍇ
				}
				M[i].texName = mqoSetTexturePool(path_tex,path_alp,alpha);
			}
			else {
				M[i].texName = mqoSetTexturePool(path_tex,NULL,alpha);
			}
		}
	}

	mqoMakeObjectsEx( mqoobj, obj, n_obj, M, n_mat, scale, alpha );

	// �I�u�W�F�N�g�̃f�[�^�̊J��
	for (i=0; i<n_obj; i++) {
		free(obj[i].V);
		free(obj[i].F);
	}

	// �}�e���A���̊J��
	free(M);

	return 1;
}


/*=========================================================================
�y�֐��zmqoCreateList
�y�p�r�zMQO�I�u�W�F�N�g���w�萔�m�ۂ���
�y�����znum		MQO�I�u�W�F�N�g�̐�

�y�ߒl�zMQO�I�u�W�F�N�g
=========================================================================*/

MQO_OBJECT* mqoCreateList(int num)
{
	MQO_OBJECT *obj;

	// ����������ĂȂ������珉����
	if ( ! l_GLMetaseqInitialized ) mqoInit();

	// �̈�m�ۂƏ�����
	obj = (MQO_OBJECT *)malloc(sizeof(MQO_OBJECT)*num);
	memset(obj, 0, sizeof(MQO_OBJECT)*num);

	return obj;
}


/*=========================================================================
�y�֐��zmqoCreateListObject
�y�p�r�z���^�Z�R�C�A�t�@�C��(*.mqo)����MQO�I�u�W�F�N�g�z����쐬����

�y�����zmqoobj		MQO�I�u�W�F�N�g
		i			�ǂݍ��ݐ�ԍ��ii�Ԗڂ�MQO�t�@�C����ǂݍ��ށj
		filename	�t�@�C���̃p�X
		scale		�g�嗦
		alpha		�A���t�@�w��i�S�̂̃A���t�@�l���w��i0�`255�j�j

�y�ߒl�z�X�e�[�^�X�@���F�ُ�@�O�F����
=========================================================================*/

int mqoCreateListObject(MQO_OBJECT *mqoobj, int i, char *filename, double scale, unsigned char alpha )
{
	int ret;
	ret = 0;
	if ( mqoobj == (MQO_OBJECT *)NULL ) return -1;
	if (! mqoLoadFile(&mqoobj[i], filename, scale, alpha)) ret = -1; 
	return ret;
}


/*=========================================================================
�y�֐��zmqoCallListObject
�y�p�r�zMQO�I�u�W�F�N�g��OpenGL�̉�ʏ�ɌĂяo��
�y�����z
		mqoobj		MQO�I�u�W�F�N�g�z��
		num			�z��ԍ� (0�`�j

�y�ߒl�z�Ȃ�
=========================================================================*/

void mqoCallListObject(MQO_OBJECT mqoobj[],int num)
{

	MQO_INNER_OBJECT	*obj;
	MQO_MATERIAL		*mat;
	GLfloat				matenv[4];
	GLint				bindGL_TEXTURE_2D	= 0;
	GLboolean			isGL_TEXTURE_2D		= GL_FALSE;
	GLboolean			isGL_BLEND			= GL_FALSE;
	GLint				blendGL_SRC_ALPHA	= 0;
	GLint				intFrontFace;

	int		o, m, offset;
	double	dalpha;
	char	*base;

	if ( mqoobj == NULL) return;

	glPushMatrix();
		//���^�Z�R�͒��_�̕��т��\�ʂ���݂ĉE���
		glGetIntegerv(GL_FRONT_FACE,&intFrontFace);
		glFrontFace(GL_CW);
		dalpha = (double)mqoobj[num].alpha/(double)255;

		for ( o=0; o<mqoobj[num].objnum; o++ ) {	// �����I�u�W�F�N�g���[�v

			obj = &mqoobj[num].obj[o];
			if ( ! obj->isVisible ) continue;
			glShadeModel(((obj->isShadingFlat))?GL_FLAT:GL_SMOOTH);

			for ( m = 0; m < obj->matnum; m++ ) {	//�}�e���A�����[�v

				mat = &obj->mat[m];
				if ( mat->datanum == 0 ) continue;

				if ( mat->isValidMaterialInfo ) {	// �}�e���A���̏��ݒ�
					memcpy(matenv,mat->dif,sizeof(matenv));
					matenv[3] *= dalpha;
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matenv);
					memcpy(matenv,mat->amb,sizeof(matenv));
					matenv[3] *= dalpha;
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matenv);
					memcpy(matenv,mat->spc,sizeof(matenv));
					matenv[3] *= dalpha;
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matenv);
					memcpy(matenv,mat->emi,sizeof(matenv));
					matenv[3] *= dalpha;
					glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matenv);
					glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat->power);
				}

				if ( mat->isUseTexture) {	// �e�N�X�`��������ꍇ
					glEnableClientState( GL_VERTEX_ARRAY );
					glEnableClientState( GL_NORMAL_ARRAY );
					glEnableClientState( GL_TEXTURE_COORD_ARRAY );

					isGL_TEXTURE_2D = glIsEnabled(GL_TEXTURE_2D);
					isGL_BLEND = glIsEnabled(GL_BLEND);
					glGetIntegerv(GL_TEXTURE_BINDING_2D,&bindGL_TEXTURE_2D);
//					glGetIntegerv(GL_BLEND_SRC_ALPHA,&blendGL_SRC_ALPHA);

					glEnable(GL_TEXTURE_2D);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

					glBindTexture(GL_TEXTURE_2D,mat->texture_id);

					if ( g_isVBOSupported ) {	// ���_�o�b�t�@�g�p
						base = (char *)NULL;	// �A�h���X��NULL���擪
						glBindBufferARB( GL_ARRAY_BUFFER_ARB, mat->VBO_id ); // ���_�o�b�t�@�����т���
					}
					else {
						// ���_�z��̎��́A�A�h���X�����̂܂ܓ����
						base = (char *)mat->vertex_t[0].point;
					}

					// ���_�z���ݒ�
					offset = (int)( (char *)mat->vertex_t[0].point - (char *)mat->vertex_t[0].point );
					glVertexPointer( 3, GL_FLOAT, sizeof(VERTEX_TEXUSE) , base + offset );

					// �e�N�X�`�����W�z���ݒ�
					offset = (int)((char *)mat->vertex_t[0].uv-(char *)mat->vertex_t[0].point);
					glTexCoordPointer( 2, GL_FLOAT, sizeof(VERTEX_TEXUSE) , base + offset );

					// �@���z���ݒ�
					offset = (int)((char *)mat->vertex_t[0].normal-(char *)mat->vertex_t[0].point);
					glNormalPointer( GL_FLOAT, sizeof(VERTEX_TEXUSE) , base+offset );

					// �F�ݒ�
					glColor4f(mat->color[0],mat->color[1],mat->color[2],mat->color[3]);

					// �`����s
					glDrawArrays( GL_TRIANGLES, 0, mat->datanum );

					glBindTexture(GL_TEXTURE_2D,bindGL_TEXTURE_2D);
					if( isGL_BLEND == GL_FALSE ) glDisable(GL_BLEND);
					if( isGL_TEXTURE_2D == GL_FALSE ) glDisable(GL_TEXTURE_2D);

					if ( g_isVBOSupported ) {						// ���_�o�b�t�@�g�p
						glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );	// ���_�o�b�t�@���f�t�H���g��
					}

					glDisableClientState( GL_VERTEX_ARRAY );
					glDisableClientState( GL_NORMAL_ARRAY );
					glDisableClientState( GL_TEXTURE_COORD_ARRAY );
				}
				else {	// �e�N�X�`�����Ȃ��ꍇ

					glEnableClientState( GL_VERTEX_ARRAY );
					glEnableClientState( GL_NORMAL_ARRAY );
				//	glEnableClientState( GL_COLOR_ARRAY );

					isGL_BLEND = glIsEnabled(GL_BLEND);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

					if ( g_isVBOSupported ) {	// ���_�o�b�t�@�g�p
						base = (char *)NULL;
						glBindBufferARB( GL_ARRAY_BUFFER_ARB, mat->VBO_id );
					}
					else {
						base = (char *)mat->vertex_p[0].point;
					}

					// ���_�z���ݒ�
					offset = (int)((char *)mat->vertex_p[0].point-(char *)mat->vertex_p[0].point);
					glVertexPointer( 3, GL_FLOAT, sizeof(VERTEX_NOTEX) , base+offset );

					// �@���z���ݒ�
					offset = (int)((char *)mat->vertex_p[0].normal-(char *)mat->vertex_p[0].point);
					glNormalPointer( GL_FLOAT, sizeof(VERTEX_NOTEX) , base+offset );

					// �F�ݒ�
					glColor4f(mat->color[0],mat->color[1],mat->color[2],mat->color[3]);
				//	offset = (int)((char *)mat->vertex_p[0].color-(char *)mat->vertex_p[0].point);
				//	glColorPointer(4,GL_FLOAT,sizeof(VERTEX_NOTEX),base+offset);

					// �`����s
					glDrawArrays( GL_TRIANGLES, 0, mat->datanum );

					if( isGL_BLEND == GL_FALSE ) glDisable(GL_BLEND);
					if ( g_isVBOSupported ) {						// ���_�o�b�t�@�g�p
						glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );	// ���_�o�b�t�@���f�t�H���g��
					}

				//	glDisableClientState( GL_COLOR_ARRAY );
					glDisableClientState( GL_VERTEX_ARRAY );
					glDisableClientState( GL_NORMAL_ARRAY );

				}
			}
		}

		//���^�Z�R�͒��_�̕��т��\�ʂ���݂ĉE���i���̐ݒ�ɂ��ǂ��j
		glFrontFace(intFrontFace);
	glPopMatrix();
}


/*=========================================================================
�y�֐��zmqoGetDirectory
�y�p�r�z�t�@�C�������܂ރp�X�����񂩂�f�B���N�g���̃p�X�݂̂𒊏o����
�y�����z
		*path_file	�t�@�C�������܂ރp�X������i���́j
		*path_dir	�t�@�C�������������p�X������i�o�́j

�y�ߒl�z�Ȃ�
�y�d�l�z��F
		"C:/data/file.bmp" �� "C:/data/"
		"data/file.mqo"    �� "data/"
=========================================================================*/

void mqoGetDirectory(const char *path_file, char *path_dir)
{
	char *pStr;
	int len;

	pStr = MAX( strrchr(path_file,'\\'), strrchr(path_file,'/') );
	len = MAX((int)(pStr-path_file)+1,0);
	strncpy(path_dir,path_file,len);
	path_dir[len] = (char)0;
}


/*=========================================================================
�y�֐��zmqoSnormal
�y�p�r�z�@���x�N�g�������߂�
�y�����z
		A		3�������W��̓_A
		B		3�������W��̓_B
		C		3�������W��̓_C
		*normal	�x�N�g��BA�ƃx�N�g��BC�̖@���x�N�g���i�E�˂������j

�y�ߒl�z�Ȃ�
�y�d�l�z���^�Z�R�C�A�ɂ����Ėʂ��\�����钸�_�̔ԍ��́C�\���ʂ��猩��
		���v���ɋL�q���Ă���D�܂�C���_A,B,C ���������Ƃ��C
		���߂�ׂ��@����BA��BC�̊O�ςɂ���ċ��߂���
=========================================================================*/

void mqoSnormal(glPOINT3f A, glPOINT3f B, glPOINT3f C, glPOINT3f *normal)
{
	double norm;
	glPOINT3f vec0,vec1;

	// �x�N�g��BA
	vec0.x = A.x - B.x; 
	vec0.y = A.y - B.y;
	vec0.z = A.z - B.z;

	// �x�N�g��BC
	vec1.x = C.x - B.x;
	vec1.y = C.y - B.y;
	vec1.z = C.z - B.z;

	// �@���x�N�g��
	normal->x = vec0.y * vec1.z - vec0.z * vec1.y;
	normal->y = vec0.z * vec1.x - vec0.x * vec1.z;
	normal->z = vec0.x * vec1.y - vec0.y * vec1.x;

	// ���K��
	norm = normal->x * normal->x + normal->y * normal->y + normal->z * normal->z;
	norm = sqrt ( norm );

	normal->x /= norm;
	normal->y /= norm;
	normal->z /= norm;
}


/*=========================================================================
�y�֐��zmqoReadMaterial
�y�p�r�z�}�e���A�����̓ǂݍ���
�y�����z
		fp		�t�@�C���|�C���^
		M		�}�e���A���z��

�y�ߒl�z�Ȃ�
�y�d�l�zmqoCreateModel(), mqoCreateSequence()�̃T�u�֐��D
=========================================================================*/

void mqoReadMaterial(FILE *fp, MQO_MATDATA M[])
{
	GLfloat		dif, amb, emi, spc;
	glCOLOR4f	c;
	char		buf[SIZE_STR];
	char		*pStrEnd, *pStr;
	int			len;
	int			i = 0;

	while (1) {
		fgets(buf,SIZE_STR,fp);	// �s�ǂݍ���
		if (strstr(buf,"}")) break;

		pStr = strstr(buf,"col(");	// �ގ����ǂݔ�΂�
		sscanf( pStr,
				"col(%f %f %f %f) dif (%f) amb(%f) emi(%f) spc(%f) power(%f)",
				&c.r, &c.g, &c.b, &c.a, &dif, &amb, &emi, &spc, &M[i].power );

		// ���_�J���[
		M[i].col = c;

		// �g�U��
		M[i].dif[0] = dif * c.r;
		M[i].dif[1] = dif * c.g;
		M[i].dif[2] = dif * c.b;
		M[i].dif[3] = c.a;

		// ���͌�
		M[i].amb[0] = amb * c.r;
		M[i].amb[1] = amb * c.g;
		M[i].amb[2] = amb * c.b;
		M[i].amb[3] = c.a;

		// ���ȏƖ�
		M[i].emi[0] = emi * c.r;
		M[i].emi[1] = emi * c.g;
		M[i].emi[2] = emi * c.b;
		M[i].emi[3] = c.a;

		// ���ˌ�
		M[i].spc[0] = spc * c.r;
		M[i].spc[1] = spc * c.g;
		M[i].spc[2] = spc * c.b;
		M[i].spc[3] = c.a;
		
		// tex�F�͗l�}�b�s���O��
		if ( (pStr = strstr(buf,"tex(")) != NULL ) {
			M[i].useTex = TRUE;

			pStrEnd = strstr(pStr,")")-1;
			len = pStrEnd - (pStr+5);
			strncpy(M[i].texFile,pStr+5,len);
			M[i].texFile[len] = (char)0;
			if ( (pStr = strstr(buf,"aplane(")) != NULL ) {
				pStrEnd = strstr(pStr,")")-1;
				len = pStrEnd - (pStr+8);
				strncpy(M[i].alpFile,pStr+8,len);
				M[i].alpFile[len] = (char)0;
			}
			else {
				M[i].alpFile[0] = (char)0;
			}

		} else {
			M[i].useTex = FALSE;
			M[i].texFile[0] = (char)0;
			M[i].alpFile[0] = (char)0;
		}

		i++;
	}

}


/*=========================================================================
�y�֐��zmqoReadVertex
�y�p�r�z���_���̓ǂݍ���
�y�����z
		fp		���݃I�[�v�����Ă��郁�^�Z�R�C�A�t�@�C���̃t�@�C���|�C���^
		V		���_���i�[����z��
		
�y�ߒl�z�Ȃ�
�y�d�l�zmqoReadObject()�̃T�u�֐�
=========================================================================*/

void mqoReadVertex(FILE *fp, glPOINT3f V[])
{
	char buf[SIZE_STR];
	int  i=0;

	while (1) {
		fgets(buf,SIZE_STR,fp);
		if (strstr(buf,"}")) break;
		sscanf(buf,"%f %f %f",&V[i].x,&V[i].y,&V[i].z);
		i++;
	}
}


/*=========================================================================
�y�֐��zmqoReadBVertex
�y�p�r�z�o�C�i���`���̒��_����ǂݍ���
�y�����z
		fp		���݃I�[�v�����Ă��郁�^�Z�R�C�A�t�@�C���̃t�@�C���|�C���^
		V		���_���i�[����z��

�y�ߒl�z���_��
�y�d�l�zmqoReadObject()�̃T�u�֐�
=========================================================================*/

int mqoReadBVertex(FILE *fp, glPOINT3f V[])
{
	int n_vertex,i;
	float *wf;
	int size;
	char cw[256];
	char *pStr;

	fgets(cw,sizeof(cw),fp);
	if ( (pStr = strstr(cw,"Vector")) != NULL ) {
		sscanf(pStr,"Vector %d [%d]",&n_vertex,&size);	// ���_���A�f�[�^�T�C�Y��ǂݍ���
	}
	else {
		return -1;
	}
	//MQO�t�@�C���̃o�C�i�����_�f�[�^��intel�`���i���g���G�f�B�A���j
	wf = (float *)malloc(size);
	fread(wf,size,1,fp);
	for ( i = 0; i < n_vertex; i++ ) {
		V[i].x = wf[i*3+0];
		V[i].y = wf[i*3+1];
		V[i].z = wf[i*3+2];
#if DEF_IS_LITTLE_ENDIAN
#else
		endianConverter((void *)&V[i].x,sizeof(V[i].x));
		endianConverter(&V[i].y,sizeof(V[i].y));
		endianConverter(&V[i].z,sizeof(V[i].z));
#endif
	}
	free(wf);

	// "}"�܂œǂݔ�΂�
	{
		char buf[SIZE_STR];
		while (1) {
			fgets(buf,SIZE_STR,fp);
			if (strstr(buf,"}")) break;
		}
	}

	return n_vertex;
}


/*=========================================================================
�y�֐��zmqoReadFace
�y�p�r�z�ʏ��̓ǂݍ���
�y�����z
		fp		�t�@�C���|�C���^
		F		�ʔz��
		
�y�ߒl�z�Ȃ�
�y�d�l�zmqoReadObject()�̃T�u�֐�
=========================================================================*/

void mqoReadFace(FILE *fp, MQO_FACE F[])
{
	char buf[SIZE_STR];
	char *pStr;
	int  i=0;

	while (1) {
		fgets(buf,SIZE_STR,fp);
		if (strstr(buf,"}")) break;

		// �ʂ��\�����钸�_��
		sscanf(buf,"%d",&F[i].n);

		// ���_(V)�̓ǂݍ���
		if ( (pStr = strstr(buf,"V(")) != NULL ) {
			switch (F[i].n) {
				case 3:
//���^�Z�R�͒��_�̕��т��\�ʂ���݂ĉE���
//�ǂݍ��ݎ��ɕ��בւ�����@������B���ǁA�\�ʂ̐ݒ��
//glFrontFace�ŕς���ق����X�}�[�g�H
					sscanf(pStr,"V(%d %d %d)",&F[i].v[0],&F[i].v[1],&F[i].v[2]);
//					sscanf(pStr,"V(%d %d %d)",&F[i].v[2],&F[i].v[1],&F[i].v[0]);
					break;
				case 4:
					sscanf(pStr,"V(%d %d %d %d)",&F[i].v[0],&F[i].v[1],&F[i].v[2],&F[i].v[3]);
//					sscanf(pStr,"V(%d %d %d %d)",&F[i].v[3],&F[i].v[2],&F[i].v[1],&F[i].v[0]);
					break;
				default:
					break;
			}		
		}

		// �}�e���A��(M)�̓ǂݍ���
		F[i].m = 0;
		if ( (pStr = strstr(buf,"M(")) != NULL ) {
			sscanf(pStr,"M(%d)",&F[i].m);
		}
		else { // �}�e���A�����ݒ肳��Ă��Ȃ���
			F[i].m = -1;
		}

		// UV�}�b�v(UV)�̓ǂݍ���
		if ( (pStr = strstr(buf,"UV(")) != NULL ) {
			switch (F[i].n) {
				case 3:	// ���_��3
					sscanf(pStr,"UV(%f %f %f %f %f %f)",
									&F[i].uv[0].x, &F[i].uv[0].y,
									&F[i].uv[1].x, &F[i].uv[1].y,
									&F[i].uv[2].x, &F[i].uv[2].y
									);
					break;

				case 4:	// ���_��4
					sscanf(pStr,"UV(%f %f %f %f %f %f %f %f)",
									&F[i].uv[0].x, &F[i].uv[0].y,
									&F[i].uv[1].x, &F[i].uv[1].y,
									&F[i].uv[2].x, &F[i].uv[2].y,
									&F[i].uv[3].x, &F[i].uv[3].y
									);
					break;
				default:
					break;
			}		
		}

		i++;
	}

}


/*=========================================================================
�y�֐��zmqoReadObject
�y�p�r�z�I�u�W�F�N�g���̓ǂݍ���
�y�����z
		fp		�t�@�C���|�C���^
		obj		�I�u�W�F�N�g���

�y�ߒl�z�Ȃ�
�y�d�l�z���̊֐��łP�̃I�u�W�F�N�g��񂪓ǂݍ��܂��D
=========================================================================*/

void mqoReadObject(FILE *fp, MQO_OBJDATA *obj)
{
	char buf[SIZE_STR];

	while (1) {
		fgets(buf,SIZE_STR,fp);
		if (strstr(buf,"}")) break;

		// visible
		if (strstr(buf,"visible ")) {
			sscanf(buf," visible %d", &obj->visible);
		}

		// shading
		if (strstr(buf,"shading ")) {
			sscanf(buf," shading %d", &obj->shading);
		}

		// facet
		if (strstr(buf,"facet ")) {
			sscanf(buf," facet %f", &obj->facet);
		}

		// vertex
		if (strstr(buf,"vertex ")) {
			sscanf(buf," vertex %d", &obj->n_vertex);
			obj->V = (glPOINT3f*) calloc( obj->n_vertex, sizeof(glPOINT3f) );
			mqoReadVertex(fp, obj->V);
		}
		// BVertex
		if (strstr(buf,"BVertex"))	{
			sscanf(buf," BVertex %d", &obj->n_vertex);
			obj->V = (glPOINT3f*) calloc( obj->n_vertex, sizeof(glPOINT3f) );
			mqoReadBVertex(fp,obj->V);
		}

		// face
		if (strstr(buf,"face ")) {
			sscanf(buf," face %d", &obj->n_face);
			obj->F = (MQO_FACE*) calloc( obj->n_face, sizeof(MQO_FACE) );
			mqoReadFace(fp, obj->F);
		}

	}

}


/*=========================================================================
�y�֐��zmqoMakeArray
�y�p�r�z���_�z��̍쐬
�y�����z
		mat		�}�e���A���i���̒��ɒ��_�f�[�^���܂ށj
		matpos	�ގ��ԍ�
		F		��
		fnum	�ʐ�
		V		���_
		N		�@��
		facet	�X���[�W���O�p
		mcol	�F
		scale	�X�P�[��
		alpha	�A���t�@

�y�ߒl�z�Ȃ�
�y�d�l�z���_�z��͂��ׂĎO�p�ɂ���̂ŁA�l�p�͎O�p���Q�ɕ���
		  0  3      0    0  3
		   ��   ���@���@�@��
		  1  2     1  2   2  
=========================================================================*/

void mqoMakeArray( MQO_MATERIAL *mat, int matpos, MQO_FACE F[], int fnum,glPOINT3f V[],
				   glPOINT3f N[], double facet, glCOLOR4f *mcol, double scale, unsigned char alpha )
{
	int f;
	int i;
	int dpos;
	double s;
	glPOINT3f normal;	// �@���x�N�g��
	
	dpos = 0;
	mat->color[0] = mcol->r;
	mat->color[1] = mcol->g;
	mat->color[2] = mcol->b;
	mat->color[3] = mcol->a;
	if ( mat->isUseTexture ) {
		for ( f = 0; f < fnum; f++ ){
			if ( F[f].m != matpos ) continue;
			if ( F[f].n == 3 ) {
				mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&normal);	// �@���x�N�g�����v�Z
				for ( i = 0; i < 3; i++ ) {
					mat->vertex_t[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_t[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_t[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_t[dpos].uv[0] = F[f].uv[i].x;
					mat->vertex_t[dpos].uv[1] = F[f].uv[i].y;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						// �X���[�W���O�p�@���i���_�@���Ɩʖ@���̊p�x�j�̂Ƃ��͖ʖ@���𒸓_�@���Ƃ���
						mat->vertex_t[dpos].normal[0] = normal.x;
						mat->vertex_t[dpos].normal[1] = normal.y;
						mat->vertex_t[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_t[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_t[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_t[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
			//�S���_�i�l�p�j�͂R���_�i�O�p�j���Q�ɕ���
			if ( F[f].n == 4 ) {
				mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&normal);	// �@���x�N�g�����v�Z
				for ( i = 0; i < 4; i++ ) {
					if ( i == 3 ) continue;
					mat->vertex_t[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_t[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_t[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_t[dpos].uv[0] = F[f].uv[i].x;
					mat->vertex_t[dpos].uv[1] = F[f].uv[i].y;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						mat->vertex_t[dpos].normal[0] = normal.x;
						mat->vertex_t[dpos].normal[1] = normal.y;
						mat->vertex_t[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_t[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_t[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_t[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
				mqoSnormal(V[F[f].v[0]],V[F[f].v[2]],V[F[f].v[3]],&normal);	// �@���x�N�g�����v�Z
				for ( i = 0; i < 4; i++ ) {
					if ( i == 1 ) continue;
					mat->vertex_t[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_t[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_t[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_t[dpos].uv[0] = F[f].uv[i].x;
					mat->vertex_t[dpos].uv[1] = F[f].uv[i].y;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						mat->vertex_t[dpos].normal[0] = normal.x;
						mat->vertex_t[dpos].normal[1] = normal.y;
						mat->vertex_t[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_t[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_t[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_t[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
		}
	}
	else {
		if ( alpha != 255 ) {
			mat->color[3] = (double)alpha/(double)255;
		}
		for ( f = 0; f < fnum; f++ ){
			if ( F[f].m != matpos ) continue;
			if ( F[f].n == 3 ) {
				mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&normal);		// �@���x�N�g�����v�Z
				for ( i = 0; i < 3; i++ ) {
					mat->vertex_p[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_p[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_p[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_p[dpos].normal[0] = normal.x;
					mat->vertex_p[dpos].normal[1] = normal.y;
					mat->vertex_p[dpos].normal[2] = normal.z;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						mat->vertex_p[dpos].normal[0] = normal.x;
						mat->vertex_p[dpos].normal[1] = normal.y;
						mat->vertex_p[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_p[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_p[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_p[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
			//�S���_�i�l�p�j�͂R���_�i�O�p�j���Q�ɕ���
			if ( F[f].n == 4 ) {
				mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&normal);		// �@���x�N�g�����v�Z
				for ( i = 0; i < 4; i++ ) {
					if ( i == 3 ) continue;
					mat->vertex_p[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_p[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_p[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_p[dpos].normal[0] = normal.x;
					mat->vertex_p[dpos].normal[1] = normal.y;
					mat->vertex_p[dpos].normal[2] = normal.z;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						mat->vertex_p[dpos].normal[0] = normal.x;
						mat->vertex_p[dpos].normal[1] = normal.y;
						mat->vertex_p[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_p[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_p[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_p[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
				mqoSnormal(V[F[f].v[0]],V[F[f].v[2]],V[F[f].v[3]],&normal);		// �@���x�N�g�����v�Z
				for ( i = 0; i < 4; i++ ) {
					if ( i == 1 ) continue;
					mat->vertex_p[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_p[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_p[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_p[dpos].normal[0] = normal.x;
					mat->vertex_p[dpos].normal[1] = normal.y;
					mat->vertex_p[dpos].normal[2] = normal.z;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						mat->vertex_p[dpos].normal[0] = normal.x;
						mat->vertex_p[dpos].normal[1] = normal.y;
						mat->vertex_p[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_p[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_p[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_p[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
		}
	}
}


/*=========================================================================
�y�֐��zmqoVertexNormal
�y�p�r�z���_�@���̌v�Z
�y�����z
		obj			�I�u�W�F�N�g���

�y�ߒl�z�@���z��
�y�d�l�z�S���_�̖ʂ͎O�p�`�ɕ������Čv�Z
		�߂�l�͕K���Ăяo�����ŉ���ifree�j���邱�ƁI
=========================================================================*/

glPOINT3f * mqoVertexNormal(MQO_OBJDATA *obj)
{
	int f;
	int v;
	int i;
	double len;
	glPOINT3f fnormal;	// �ʖ@���x�N�g��
	MQO_FACE *F;
	glPOINT3f *V;
	glPOINT3f *ret;
	F = obj->F;
	V = obj->V;
	ret = (glPOINT3f *)calloc(obj->n_vertex,sizeof(glPOINT3f));
	//�ʂ̖@���𒸓_�ɑ�������
	for ( f = 0; f < obj->n_face; f++ ) {
		if ( obj->F[f].n == 3 ) {
			mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&fnormal);
			for ( i = 0; i < 3; i++ ) {
				ret[F[f].v[i]].x += fnormal.x;
				ret[F[f].v[i]].y += fnormal.y;
				ret[F[f].v[i]].z += fnormal.z;
			}
		}
		if ( obj->F[f].n == 4 ) {
			mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&fnormal);
			for ( i = 0; i < 4; i++ ) {
				if ( i == 3 ) continue;
				ret[F[f].v[i]].x += fnormal.x;
				ret[F[f].v[i]].y += fnormal.y;
				ret[F[f].v[i]].z += fnormal.z;
			}
			mqoSnormal(V[F[f].v[0]],V[F[f].v[2]],V[F[f].v[3]],&fnormal);
			for ( i = 0; i < 4; i++ ) {
				if ( i == 1 ) continue;
				ret[F[f].v[i]].x += fnormal.x;
				ret[F[f].v[i]].y += fnormal.y;
				ret[F[f].v[i]].z += fnormal.z;
			}
		}
	}
	//���K��
	for ( v = 0; v < obj->n_vertex; v++ ) {
		if ( ret[v].x == 0 && ret[v].y == 0 && ret[v].z == 0 ) {
			//�ʂɎg���ĂȂ��_
			continue;
		}
		len = sqrt(ret[v].x*ret[v].x + ret[v].y*ret[v].y + ret[v].z*ret[v].z);
		if ( len != 0 ) {
			ret[v].x = ret[v].x/len;
			ret[v].y = ret[v].y/len;
			ret[v].z = ret[v].z/len;
		}
	}

	return ret;
}

/*=========================================================================
�y�֐��zmqoMakePolygon
�y�p�r�z�|���S���̐���
�y�����z
		readObj		�ǂݍ��񂾃I�u�W�F�N�g���
		mqoobj		MQO�I�u�W�F�N�g 
		N[]			�@���z��
		M[]			�}�e���A���z��
		n_mat		�}�e���A����
		scale		�X�P�[��
		alpha		�A���t�@

�y�ߒl�z�Ȃ�
=========================================================================*/

void mqoMakePolygon(MQO_OBJDATA *readObj, MQO_OBJECT *mqoobj,
					glPOINT3f N[], MQO_MATDATA M[], int n_mat, double scale, unsigned char alpha)
{

	MQO_INNER_OBJECT		*setObj;
	MQO_MATERIAL			*material;
	glCOLOR4f				defcol;
	glCOLOR4f				*pcol;
	int						f, m, *mat_vnum;
	int						fnum;
	MQO_FACE				*F;
	glPOINT3f				*V;
	double					facet;


	setObj = &mqoobj->obj[mqoobj->objnum];
	strcpy(setObj->objname,readObj->objname);
	setObj->isVisible = readObj->visible;
	setObj->isShadingFlat = (readObj->shading == 0);
	F = readObj->F;
	fnum = readObj->n_face;
	V = readObj->V;
	facet = readObj->facet;

	// face�̒��ł̃}�e���A�����̒��_�̐�
	// M=NULL�̂Ƃ��AF[].m = 0 �������Ă���
	if ( M == NULL ) n_mat = 1;

	mat_vnum = (int *)malloc(sizeof(int)*n_mat);
	memset(mat_vnum,0,sizeof(int)*n_mat);

	for ( f = 0; f < fnum; f++ ){
		if( F[f].m < 0 ) continue; // �}�e���A�����ݒ肳��Ă��Ȃ���
		if ( F[f].n == 3 ) {
			mat_vnum[F[f].m] += 3;
		}
		if ( F[f].n == 4 ) {
			//�S���_�i�l�p�j�͂R���_�i�O�p�j���Q�ɕ���
			//  0  3      0    0  3
			//   ��   ���@���@�@��
			//  1  2     1  2   2
			// �S���_�̕��ʃf�[�^��
			// �R���_�̕��ʃf�[�^���Q��
			mat_vnum[F[f].m] += 3*2;
		}
		if ( setObj->matnum < F[f].m+1 ) setObj->matnum = F[f].m+1;
	}

	// �}�e���A���ʂɒ��_�z����쐬����
	setObj->mat = (MQO_MATERIAL *)malloc(sizeof(MQO_MATERIAL)*setObj->matnum);
	memset(setObj->mat,0,sizeof(MQO_MATERIAL)*setObj->matnum);

	for ( m = 0; m < setObj->matnum; m++ ) {
		material = &setObj->mat[m];
		material->datanum = mat_vnum[m];
		material->isValidMaterialInfo = (M != NULL);

		if ( mat_vnum[m] <= 0 ) continue;
		if ( material->isValidMaterialInfo ) {
			memcpy(material->dif,M[m].dif,sizeof(material->dif));
			memcpy(material->amb,M[m].amb,sizeof(material->amb));
			memcpy(material->spc,M[m].spc,sizeof(material->spc));
			memcpy(material->emi,M[m].emi,sizeof(material->emi));
			material->power = M[m].power;
			material->isUseTexture = M[m].useTex;
			pcol = &M[m].col;
		}
		else {
			defcol.r = 1.0;
			defcol.g = 1.0;
			defcol.b = 1.0;
			defcol.a = 1.0;
			material->isUseTexture = 0;
			pcol = &defcol;
		}
		if ( material->isUseTexture ) {
			material->vertex_t = (VERTEX_TEXUSE *)calloc(material->datanum,sizeof(VERTEX_TEXUSE));
			material->texture_id = M[m].texName;
		}
		else {
			material->vertex_p = (VERTEX_NOTEX *)calloc(material->datanum,sizeof(VERTEX_NOTEX));
		}
		mqoMakeArray(material,m,F,fnum,V,N,facet,pcol,scale,alpha);
		if (g_isVBOSupported) {
			if ( material->isUseTexture ) {
				glGenBuffersARB( 1, &material->VBO_id );
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, material->VBO_id  );
				glBufferDataARB( GL_ARRAY_BUFFER_ARB, material->datanum*sizeof(VERTEX_TEXUSE), material->vertex_t, GL_STATIC_DRAW_ARB );
			}
			else {
				glGenBuffersARB( 1, &material->VBO_id );
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, material->VBO_id  );
				glBufferDataARB( GL_ARRAY_BUFFER_ARB, material->datanum*sizeof(VERTEX_NOTEX), material->vertex_p, GL_STATIC_DRAW_ARB );
			}
		}
	}
	mqoobj->objnum++;
	if ( MAX_OBJECT <= mqoobj->objnum ) {
		printf("MQO�t�@�C���ǂݍ��݁F�@�ő�I�u�W�F�N�g���𒴂��܂���[%d]\n",mqoobj->objnum);
		mqoobj->objnum = MAX_OBJECT-1;
	}

	free(mat_vnum);

}


/*=========================================================================
�y�֐��zmqoMakeObjectsEx
�y�p�r�z�I�u�W�F�N�g�̃f�[�^����|���S�����f�����쐬����
�y�����z
		mqoobj	MQO�I�u�W�F�N�g
		obj		�I�u�W�F�N�g�z��
		n_obj	�I�u�W�F�N�g�̌�
		M		�}�e���A���z��
		n_mat	�}�e���A���̌�
		scale	�g�嗦
		alpha	�A���t�@

�y�ߒl�z�Ȃ�
=========================================================================*/

void mqoMakeObjectsEx( MQO_OBJECT *mqoobj, MQO_OBJDATA obj[], int n_obj, MQO_MATDATA M[],int n_mat,
					   double scale,unsigned char alpha)
{
	int i;
	glPOINT3f *N;
	for (i=0; i<n_obj; i++) {
		N = mqoVertexNormal(&obj[i]);
		mqoMakePolygon(
				&obj[i],
				mqoobj,
				N,
				M,n_mat,
				scale,
				alpha);
		free(N);
	}
}


/*=========================================================================
�y�֐��zmqoCreateModel
�y�p�r�zMQO�t�@�C������MQO���f�����쐬����
�y�����z
		filename	MQO�t�@�C��
		scale		�g�嗦�i1.0�ł��̂܂܁j

�y�ߒl�zMQO_MODEL�iMQO���f���j
=========================================================================*/

MQO_MODEL mqoCreateModel(char *filename, double scale)
{
	MQO_MODEL ret;
	ret = mqoCreateList(1);
	if ( mqoCreateListObject(ret,1-1,filename,scale,(unsigned char)255) < 0 ) {
		mqoDeleteObject(ret,1);
		ret = NULL;
	}
	return ret;
}


/*=========================================================================
�y�֐��zmqoCreateSequenceEx
�y�p�r�z�A�Ԃ�MQO�t�@�C������MQO�V�[�P���X���쐬����
�y�����z
		format		�t�@�C�����̏���
		n_file		�t�@�C����
		scale		�g�嗦�i1.0�ł��̂܂܁j
		fade_inout	0:���̂܂܁@���F�t�F�[�h�C���@���F�t�F�[�h�A�E�g
					��Βl�͌��ʂ�������t���[����
		alpha		�A���t�@

�y�ߒl�zMQO_SEQUENCE�iMQO�V�[�P���X�j
�y���l�z�A�Ԃ�0����J�n
=========================================================================*/

MQO_SEQUENCE mqoCreateSequenceEx(const char *format, int n_file, double scale,
								 int fade_inout, unsigned char alpha)
{
	MQO_SEQUENCE retSeq;
	int iret;
	int seq;
	char filename[SIZE_STR];
	short setAlpha;
	short calAlpha;
	int frames;

	retSeq.n_frame = 0;
	if ( format == NULL ) {
		return retSeq;
	}
	calAlpha = alpha;
	frames = abs(fade_inout);
	frames = MAX(frames,n_file);
	setAlpha = (fade_inout<=0)?alpha:0;

	retSeq.model = mqoCreateList(n_file);
	for ( seq = 0; seq < frames; seq++ ) {
		if ( seq < n_file ) {
			sprintf(filename,format,seq);
		}
		if ( (fade_inout !=  0) && ((frames-1) == seq) ) {
			setAlpha = (fade_inout<0)?0:calAlpha;
		}
		iret = mqoCreateListObject(retSeq.model,seq,filename,scale,(unsigned char)setAlpha);
		if ( iret == - 1 ) {
			seq--;
			mqoClearObject(retSeq.model,seq,n_file-seq);
			break;
		}
		if ( fade_inout !=  0 ) {
			if ( fade_inout<0 ) {
				if ( (n_file-seq) <= (-1*fade_inout) ) {
					setAlpha -= (calAlpha/(-1*fade_inout));
					if ( setAlpha < 0 ) setAlpha = 0;
				}
			}
			else {
				setAlpha += (calAlpha/fade_inout);
				if ( calAlpha < setAlpha ) setAlpha = calAlpha;
			}
		}
	}
	retSeq.n_frame = seq;
	return retSeq;
}


/*=========================================================================
�y�֐��zmqoCreateSequence
�y�p�r�z�A�Ԃ�MQO�t�@�C������MQO�V�[�P���X���쐬����
�y�����z
		format		�t�@�C�����̃t�H�[�}�b�g
		n_file		�t�@�C����
		scale		�g�嗦�i1.0�ł��̂܂܁j

�y�ߒl�zMQO_SEQUENCE�iMQO�V�[�P���X�j
�y���l�z�A�Ԃ�0����J�n
=========================================================================*/

MQO_SEQUENCE mqoCreateSequence(const char *format, int n_file, double scale)
{
	return mqoCreateSequenceEx(format, n_file, scale, 0, (unsigned char)255);
}


/*=========================================================================
�y�֐��zmqoCallModel
�y�p�r�zMQO���f����OpenGL�̉�ʏ�ɌĂяo��
�y�����z
		model		MQO���f��

�y�ߒl�z�Ȃ�
=========================================================================*/

void mqoCallModel(MQO_MODEL model)
{
	mqoCallListObject(model, 0);
}


/*=========================================================================
�y�֐��zmqoCallSequence
�y�p�r�zMQO�V�[�P���X��OpenGL�̉�ʂɌĂяo��
�y�����z
		seq		MQO�V�[�P���X
		i		�t���[���ԍ�

�y�ߒl�z�Ȃ�
�y�d�l�zMQO�V�[�P���X�̒�����w�肵���t���[���ԍ��̃��f�����Ăяo��
=========================================================================*/

void mqoCallSequence(MQO_SEQUENCE seq, int i)
{
	if ( i>=0 && i<seq.n_frame ) {
		mqoCallListObject(seq.model,i);
	}
}


/*=========================================================================
�y�֐��zmqoClearObject
�y�p�r�zMQO�I�u�W�F�N�g�̃N���A
�y�����z
		object	MQO�I�u�W�F�N�g�z��
		from	�폜�J�n�ԍ��i0�`�j
		num		�폜�����

�y�ߒl�z�Ȃ�
=========================================================================*/

void mqoClearObject( MQO_OBJECT object[], int from, int num ) 
{
	int					loop, o, m;
	MQO_INNER_OBJECT	*obj;
	MQO_MATERIAL		*mat;

	if ( object == NULL ) return;

	for ( loop = from; loop < from + num; loop++ ) {
		for ( o = 0; o < (object+from)->objnum; o++ ) {

			obj = &(object+loop)->obj[o];
			for ( m = 0; m < obj->matnum; m++ ) {
				mat = &obj->mat[m];
				if ( mat->datanum <= 0 ) continue;
				if ( g_isVBOSupported ) {
					// ���_�o�b�t�@�̍폜
					glDeleteBuffersARB( 1, &mat->VBO_id );
				}

				// ���_�z��̍폜
				if ( mat->isUseTexture ) {
					if ( mat->vertex_t != NULL ) {
						free(mat->vertex_t);
						mat->vertex_t = NULL;
					}
				}
				else {
					if ( mat->vertex_p != NULL ) {
						free(mat->vertex_p);
						mat->vertex_p = NULL;
					}
				}
			}
			if ( obj->mat != NULL ) {
				free(obj->mat);
				obj->mat = NULL;
			}
			obj->matnum = 0;
		}
	}

}


/*=========================================================================
�y�֐��zmqoDeleteObject
�y�p�r�zMQO�I�u�W�F�N�g���폜����
�y�����z
		object	MQO�I�u�W�F�N�g�z��
		num		�폜�����

�y�ߒl�z�Ȃ�
=========================================================================*/

void mqoDeleteObject(MQO_OBJECT object[], int num)
{
	mqoClearObject(object,0,num);
	free(object);
}


/*=========================================================================
�y�֐��zmqoDeleteModel
�y�p�r�zMQO���f�����폜����
�y�����z
		model	MQO���f��

�y�ߒl�z�Ȃ�
�y���l�z�폜�������s�����ϐ����ė��p����\��������ꍇ��
		���̊֐��̎��s���NULL�������Ă�������
=========================================================================*/

void mqoDeleteModel(MQO_MODEL model)
{
	mqoDeleteObject(model,1);
}


/*=========================================================================
�y�֐��zmqoDeleteSequence
�y�p�r�zMQO�V�[�P���X���폜����
�y�����z
		seq		MQO�V�[�P���X

�y���l�z�폜�������s�����ϐ����ė��p����\��������ꍇ��
		���̊֐��̎��s���NULL�������Ă�������
=========================================================================*/

void mqoDeleteSequence(MQO_SEQUENCE seq)
{
	mqoDeleteObject( seq.model, seq.n_frame );
}