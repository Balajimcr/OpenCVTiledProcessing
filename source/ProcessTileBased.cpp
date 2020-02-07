#include "ProcessTileBased.h"

void TiledAlgorithm::copySourceTile(const cv::Mat& src, cv::Mat& srcTile, cv::Rect &tile)
{
	auto tl = tile.tl();
	auto br = tile.br();

	cv::Point tloffset, broffset;

	//Take care of border cases
	if (tile.x < 0)
	{
		tloffset.x = -tile.x;
		tile.x = 0;
	}

	if (tile.y < 0)
	{
		tloffset.y = -tile.y;
		tile.y = 0;
	}

	if (br.x >= src.cols)
	{
		broffset.x = br.x - src.cols + 1;
		tile.width -= broffset.x;
	}

	if (br.y >= src.rows)
	{
		broffset.y = br.y - src.rows + 1;
		tile.height -= broffset.y;
	}

	// If any of the tile sides exceed mSource image boundary we must use copyMakeBorder to make proper paddings for this side
	if (tloffset.x > 0 || tloffset.y > 0 || broffset.x > 0 || broffset.y > 0)
	{
		cv::Rect paddedTile(tile.tl(), tile.br());
		assert(paddedTile.x >= 0);
		assert(paddedTile.y >= 0);
		assert(paddedTile.br().x < src.cols);
		assert(paddedTile.br().y < src.rows);

		cv::copyMakeBorder(src(paddedTile), srcTile, tloffset.y, broffset.y, tloffset.x, broffset.x, m_BorderType);
	}
	else
	{
		// Entire tile (with paddings lies inside image and it's safe to just take a region:
		src(tile).copyTo(srcTile);
	}
}

void TiledAlgorithm::copyTileToResultImage(const cv::Mat& tileImage, cv::Mat& resultImage, cv::Rect resultRoi)
{
	cv::Rect srcTile(m_Padding, m_Padding, m_TileSize, m_TileSize);

	auto br = resultRoi.br();

	if (br.x >= resultImage.cols)
	{
		resultRoi.width -= br.x - resultImage.cols;
		srcTile.width -= br.x - resultImage.cols;
	}

	if (br.y >= resultImage.rows)
	{
		resultRoi.height -= br.y - resultImage.rows;
		srcTile.height -= br.y - resultImage.rows;
	}

	cv::Mat tileView = tileImage(srcTile);
	cv::Mat dstView = resultImage(resultRoi);

	assert(tileView.rows == dstView.rows);
	assert(tileView.cols == dstView.cols);

	tileView.copyTo(dstView);
}

void TiledAlgorithm::processTileImpl(const cv::Mat& tileInput, cv::Mat& tileOutput)
{
	using namespace cv;
	cv::GaussianBlur(tileInput, Buffers[1], Size(3, 3), 0);
	cv::Sobel(Buffers[1], Buffers[2], CV_8UC1, 1, 0);
	cv::Sobel(Buffers[1], Buffers[3], CV_8UC1, 0, 1);
	cv::addWeighted(Buffers[2], 1.0, Buffers[3], 1.0, 1.0, tileOutput);
}

void TiledAlgorithm::process(const cv::Mat& sourceImage, cv::Mat& resultImage)
{
	assert(!resultImage.empty());
	assert(sourceImage.rows == resultImage.rows);
	assert(sourceImage.cols == resultImage.cols);

	int rows = (sourceImage.rows / m_TileSize) + (sourceImage.rows % m_TileSize ? 1 : 0);
	int cols = (sourceImage.cols / m_TileSize) + (sourceImage.cols % m_TileSize ? 1 : 0);

	#pragma omp parallel for
	for (int rowTile = 0; rowTile < rows; rowTile++)
	{
		for (int colTile = 0; colTile < cols; colTile++)
		{
			cv::Rect srcTile(colTile * m_TileSize - m_Padding,
				rowTile * m_TileSize - m_Padding,
				m_TileSize + 2 * m_Padding,
				m_TileSize + 2 * m_Padding);

			cv::Rect dstTile(colTile * m_TileSize,
				rowTile * m_TileSize,
				m_TileSize,
				m_TileSize);

			copySourceTile(sourceImage, tileInput, srcTile);
			processTileImpl(tileInput, tileOutput);
			copyTileToResultImage(tileOutput, resultImage, dstTile);
			//cv::imshow("resultImage", resultImage);
			//cv::waitKey();
		}
	}
}
