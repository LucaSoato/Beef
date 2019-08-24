#include "ImageData.h"
#include "ImageUtils.h"

USING_NS_BF;

ImageData::ImageData()
{
	mSrcData = NULL;
	mOwnsSrcData = false;
	mKeepSrcDataValid = false;
	mBits = NULL;
	mHWBitsType = 0;
	mHWBitsLength = 0;
	mHWBits = NULL;
	mX = 0;
	mY = 0;
	mWidth = 0;
	mHeight = 0;
	mWantsAlphaPremultiplied = true;
	mAlphaPremultiplied = false;
	mIsAdditive = false;
	mSrcDataLen = 0;		
}

ImageData::~ImageData()
{	
	delete [] mBits;
	delete [] mSrcData;
}

void ImageData::SwapRAndB()
{
	int aSize = mWidth*mHeight;
	uint32* aPtr = mBits;
	for (int i = 0; i < aSize; i++)
	{
		uint32 aColor = *aPtr;

		int a = (aColor & 0xFF000000) >> 24;
		int r = (aColor & 0x00FF0000) >> 16;
		int g = (aColor & 0x0000FF00) >> 8;
		int b = (aColor & 0x000000FF);

		*(aPtr++) = (a << 24) | (b << 16) | (g << 8) | r;
	}
}

void ImageData::CreateNew(int x, int y, int width, int height, bool clear)
{
	CreateNew(width, height, clear);
	mX = x;
	mY = y;
}

void ImageData::CreateNew(int width, int height, bool clear)
{
	mWidth = width;
	mHeight = height;
	mBits = new uint32[mWidth*mHeight];
	if (clear)
		memset(mBits, 0, mWidth*mHeight*sizeof(uint32));
}

void ImageData::Fill(uint32 color)
{
	int size = mWidth*mHeight;
	for (int i = 0; i < size; i++)
		mBits[i] = color;
}

ImageData* ImageData::Duplicate()
{
	ImageData* copy = new ImageData();
	copy->CreateNew(mWidth, mHeight);
	copy->mX = mX;
	copy->mY = mY;
	copy->mAlphaPremultiplied = mAlphaPremultiplied;
	copy->mIsAdditive = mIsAdditive;
	memcpy(copy->mBits, mBits, mWidth*mHeight*sizeof(uint32));
	return copy;
}

bool ImageData::LoadFromFile(const StringImpl& path)
{	
	int size = 0;
	uint8* aData = LoadBinaryData(path, &size);
	if (aData == NULL)
		return NULL;
	SetSrcData(aData, size);
	bool result = ReadData();
	if (mKeepSrcDataValid)
	{
		mOwnsSrcData = true;
	}
	else
	{
		delete [] mSrcData;
		mSrcData = NULL;
	}
	return result;
}

void ImageData::SetSrcData(uint8* data, int dataLen)
{
	mSrcData = data;
	mSrcDataLen = dataLen;
}

void ImageData::PremultiplyAlpha()
{
	if (mBits == NULL)
		return;

	if (!mAlphaPremultiplied)
	{
		mAlphaPremultiplied = true;
		int size = mWidth*mHeight;
		for (int i = 0; i < size; i++)
		{
			PackedColor* packedColor = (PackedColor*) (mBits + i);
			packedColor->r = (packedColor->r * packedColor->a) / 255;
			packedColor->g = (packedColor->g * packedColor->a) / 255;
			packedColor->b = (packedColor->b * packedColor->a) / 255;

			if (mIsAdditive)
				packedColor->a = 0;
		}
	}
}