#pragma once

#include "opencv2/opencv.hpp"
#include "opencv2/opencv_modules.hpp"

class TiledAlgorithm
{
public:
	TiledAlgorithm(cv::Size ImageSize, int tileSize, int padding, int borderType, int NoOfBufferImages)
		: m_TileSize(tileSize)
		, m_ImageSize(ImageSize)
		, m_Padding(padding)
		, m_BorderType(borderType)
		, m_NoOfBufferImages(NoOfBufferImages)
	{
		int size = tileSize + padding * 2;
		CV_Assert(m_NoOfBufferImages < MaxBuffers);
		
		// Allocate all buffer as continuous array
		m_Buffer.create(size * m_NoOfBufferImages, size, CV_8UC1);
		
		// Create views to sub-regions of m_Buffer
		for (size_t i = 0; i < m_NoOfBufferImages; i++)
		{
			Buffers[i] = m_Buffer.rowRange(i*size, (i + 1)*size);
		}
		tileInput = Buffers[0];
		tileOutput = Buffers[m_NoOfBufferImages-1];
	}

	void process(const cv::Mat& sourceImage, cv::Mat& resultImage);

private:

	void copySourceTile(const cv::Mat& src, cv::Mat& srcTile, cv::Rect &tile);

	void processTileImpl(const cv::Mat& tileInput, cv::Mat& tileOutput);

	void copyTileToResultImage(const cv::Mat& tileImage, cv::Mat& resultImage, cv::Rect resultRoi);

	int m_TileSize;
	int m_Padding;
	int m_BorderType;
	int m_NoOfBufferImages;
	static const int MaxBuffers = 6;

	cv::Size m_ImageSize;
	cv::Mat m_Buffer;
	cv::Mat Buffers[MaxBuffers];
	cv::Mat tileInput, tileOutput;

};
