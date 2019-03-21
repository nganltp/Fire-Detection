#pragma once
#include "package_bgs/bgslibrary.h"
#define GetTGMTbgs TGMTbgs::GetInstance

#ifdef _MANAGED
using namespace System::Drawing;

namespace TGMT
{
	public ref class TGMTbgsBridge
	{
	public:
		void Init(int type);
		void Process(Bitmap^ bmp);
		//void Process(System::String^ videoPath);
	};
}
#endif

class TGMTbgs
{
	static TGMTbgs* instance;
	IBGS* m_bgs;
public:
	enum BgsType
	{
		bgs_FrameDifference,
		bgs_StaticFrameDifference,
		bgs_WeightedMovingMean,
		bgs_WeightedMovingVariance,
		bgs_MixtureOfGaussianV2,
		bgs_AdaptiveBackgroundLearning,
		bgs_AdaptiveSelectiveBackgroundLearning,
		bgs_KNN,
		bgs_DPAdaptiveMedian,
		bgs_DPGrimsonGMM,
		bgs_DPZivkovicAGMM,
		bgs_DPMean,
		bgs_DPWrenGA,
		bgs_DPPratiMediod,
		bgs_DPEigenbackground,
		bgs_DPTexture,
		bgs_T2FGMM_UM,
		bgs_T2FGMM_UV,
		bgs_T2FMRF_UM,
		bgs_T2FMRF_UV,
		bgs_FuzzySugenoIntegral,
		bgs_FuzzyChoquetIntegral,
		bgs_MultiLayer,
		bgs_PixelBasedAdaptiveSegmenter,
		bgs_LBSimpleGaussian,
		bgs_LBFuzzyGaussian,
		bgs_LBMixtureOfGaussians,
		bgs_LBAdaptiveSOM,
		bgs_LBFuzzyAdaptiveSOM,
		bgs_LBP_MRF,
		bgs_VuMeter,
		bgs_KDE,
		bgs_IndependentMultimodal,
		bgs_MultiCue,
		bgs_SigmaDelta,
		bgs_SuBSENSE,
		bgs_LOBSTER,
		bgs_PAWCS,
		bgs_TwoPoints,
		bgs_ViBe,
		bgs_CodeBook
	};

	TGMTbgs();
	~TGMTbgs();




	static TGMTbgs* GetInstance()
	{
		if (!instance)
			instance = new TGMTbgs();
		return instance;
	}


	void Init(BgsType type);
	void Process(cv::Mat matInput);
	//void Process(std::string videoPath);


	static void main(int argc, char** argv);
};

