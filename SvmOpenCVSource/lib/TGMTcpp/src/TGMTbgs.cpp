#include "TGMTbgs.h"
#include "stdafx.h"

#include "TGMTutil.h"
//#include "TGMTdebugger.h"
#include "TGMTfile.h"
#include "TGMTutil.h"
#include "TGMTvideo.h"
#include "TGMTConfig.h"
#include "TGMTcamera.h"

#ifdef _MANAGED
#include "TGMTbridge.h"

using namespace TGMT;

#endif

TGMTbgs* TGMTbgs::instance = NULL;
int m_frameAmount = 0;
int m_frameCount = 1;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGMTbgs::TGMTbgs()
{
	TGMTfile::CreateDir("bgsconfig");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGMTbgs::~TGMTbgs()
{
	delete m_bgs;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OnVideoFrameBgs(cv::Mat frame)
{	
	if (m_frameCount == m_frameAmount)
	{
		cout << "\n";
		//STOP_AND_PRINT_COUNT_TIME("BGS");
	}
	else
	{
		GetTGMTbgs()->Process(frame);
		std::cout << "\r" << m_frameCount++ << " / " << m_frameAmount;
		cv::waitKey(1);		
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTbgs::Init(BgsType type)
{
	//if (m_bgs != nullptr)
	//{
	//	delete m_bgs;
	//}

	switch (type)
	{
	case bgs_FrameDifference:
		m_bgs = new FrameDifference();
		break;
	case bgs_StaticFrameDifference:
		m_bgs = new StaticFrameDifference;
		break;
	case bgs_WeightedMovingMean:
		m_bgs = new WeightedMovingMean;
		break;
	case bgs_WeightedMovingVariance:
		m_bgs = new WeightedMovingVariance;
		break;
	case bgs_MixtureOfGaussianV2:
		m_bgs = new MixtureOfGaussianV2;
		break;
	case bgs_AdaptiveBackgroundLearning:
		m_bgs = new AdaptiveBackgroundLearning;
		break;
	case bgs_AdaptiveSelectiveBackgroundLearning:
		m_bgs = new AdaptiveSelectiveBackgroundLearning;
		break;
	case bgs_KNN:
		m_bgs = new KNN; // only on OpenCV 3.x
		break;
	case bgs_DPAdaptiveMedian:
		m_bgs = new DPAdaptiveMedian;
		break;
	case bgs_DPGrimsonGMM:
		m_bgs = new DPGrimsonGMM;
		break;
	case bgs_DPZivkovicAGMM:
		m_bgs = new DPZivkovicAGMM;
		break;
	case bgs_DPMean:
		m_bgs = new DPMean;
		break;
	case bgs_DPWrenGA:
		m_bgs = new DPWrenGA;
		break;
	case bgs_DPPratiMediod:
		m_bgs = new DPPratiMediod;
		break;
	case bgs_DPEigenbackground:
		m_bgs = new DPEigenbackground;
		break;
	case bgs_DPTexture:
		m_bgs = new DPTexture;
		break;
	case bgs_T2FGMM_UM:
		m_bgs = new T2FGMM_UM;
		break;
	case bgs_T2FGMM_UV:
		m_bgs = new T2FGMM_UV;
		break;
	case bgs_T2FMRF_UM:
		m_bgs = new T2FMRF_UM;
		break;
	case bgs_T2FMRF_UV:
		m_bgs = new T2FMRF_UV;
		break;
	case bgs_FuzzySugenoIntegral:
		m_bgs = new FuzzySugenoIntegral;
		break;
	case bgs_FuzzyChoquetIntegral:
		m_bgs = new FuzzyChoquetIntegral;
		break;
	case bgs_MultiLayer:
		m_bgs = new MultiLayer;
		break;
	case bgs_PixelBasedAdaptiveSegmenter:
		m_bgs = new PixelBasedAdaptiveSegmenter;
		break;
	case bgs_LBSimpleGaussian:
		m_bgs = new LBSimpleGaussian;
		break;
	case bgs_LBFuzzyGaussian:
		m_bgs = new LBFuzzyGaussian;
		break;
	case bgs_LBMixtureOfGaussians:
		m_bgs = new LBMixtureOfGaussians;
		break;
	case bgs_LBAdaptiveSOM:
		m_bgs = new LBAdaptiveSOM;
		break;
	case bgs_LBFuzzyAdaptiveSOM:
		m_bgs = new LBFuzzyAdaptiveSOM;
		break;
	case bgs_LBP_MRF:
		m_bgs = new LBP_MRF;
		break;
	case bgs_VuMeter:
		m_bgs = new VuMeter;
		break;
	case bgs_KDE:
		m_bgs = new KDE;
		break;
	case bgs_IndependentMultimodal:
		m_bgs = new IndependentMultimodal;
		break;
	case bgs_MultiCue:
		m_bgs = new MultiCue;
		break;
	case bgs_SigmaDelta:
		m_bgs = new SigmaDelta;
		break;
	case bgs_SuBSENSE:
		m_bgs = new SuBSENSE;
		break;
	case bgs_LOBSTER:
		m_bgs = new LOBSTER;
		break;
	case bgs_PAWCS:
		m_bgs = new PAWCS;
		break;
	case bgs_TwoPoints:
		m_bgs = new TwoPoints;
		break;
	case bgs_ViBe:
		m_bgs = new ViBe;
		break;
	case bgs_CodeBook:
		m_bgs = new CodeBook;
		break;
	default:
		m_bgs = new FrameDifference;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTbgs::Process(cv::Mat matInput)
{
	cv::Mat img_mask;
	cv::Mat img_bkgmodel;
	m_bgs->process(matInput, img_mask, img_bkgmodel); // by default, it shows automatically the foreground mask image
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OnCameraFrameBgs(cv::Mat frame)
{
	GetTGMTbgs()->Process(frame);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MANAGED

void TGMTbgsBridge::Init(int type)
{
	GetTGMTbgs()->Init((TGMTbgs::BgsType)type);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTbgsBridge::Process(Bitmap^ bmp)
{
	cv::Mat mat = TGMTbridge::BitmapToMat(bmp);
	GetTGMTbgs()->Process(mat);
	cv::waitKey(1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void TGMTbgsBridge::Process(System::String^ videoPath)
//{
//
//}
#endif

void TGMTbgs::main(int argc, char** argv)
{
	GetTGMTConfig()->LoadSettingFromFile("bgs_config.json");


	/* Background Subtraction Methods */
	//int idx = 1;


	//PrintMessage("Select method:");
	//PrintMessage("%d FrameDifference", idx++);
	//PrintMessage("%d StaticFrameDifference", idx++);
	//PrintMessage("%d WeightedMovingMean", idx++);
	//PrintMessage("%d WeightedMovingVariance", idx++);
	//PrintMessage("%d MixtureOfGaussianV2", idx++);
	//PrintMessage("%d AdaptiveBackgroundLearning", idx++);
	//PrintMessage("%d AdaptiveSelectiveBackgroundLearning", idx++);
	//PrintMessage("%d KNN", idx++);
	//PrintMessage("%d DPAdaptiveMedian", idx++);
	//PrintMessage("%d DPGrimsonGMM", idx++);
	//PrintMessage("%d DPZivkovicAGMM", idx++);
	//PrintMessage("%d DPMean", idx++);
	//PrintMessage("%d DPWrenGA", idx++);
	//PrintMessage("%d DPPratiMediod", idx++);
	//PrintMessage("%d DPEigenbackground", idx++);
	//PrintMessage("%d DPTexture", idx++);
	//PrintMessage("%d T2FGMM_UM", idx++);
	//PrintMessage("%d T2FGMM_UV", idx++);
	//PrintMessage("%d T2FMRF_UM", idx++);
	//PrintMessage("%d T2FMRF_UV", idx++);
	//PrintMessage("%d FuzzySugenoIntegral", idx++);
	//PrintMessage("%d FuzzyChoquetIntegral", idx++);
	//PrintMessage("%d MultiLayer", idx++);
	//PrintMessage("%d PixelBasedAdaptiveSegmenter", idx++);
	//PrintMessage("%d LBSimpleGaussian", idx++);
	//PrintMessage("%d LBFuzzyGaussian", idx++);
	//PrintMessage("%d LBMixtureOfGaussians", idx++);
	//PrintMessage("%d LBAdaptiveSOM", idx++);
	//PrintMessage("%d LBFuzzyAdaptiveSOM", idx++);
	//PrintMessage("%d LBP_MRF", idx++);
	//PrintMessage("%d VuMeter", idx++);
	//PrintMessage("%d KDE", idx++);
	//PrintMessage("%d IndependentMultimodal", idx++);
	//PrintMessage("%d MultiCue", idx++);
	//PrintMessage("%d SigmaDelta", idx++);
	//PrintMessage("%d SuBSENSE", idx++);
	//PrintMessage("%d LOBSTER", idx++);
	//PrintMessage("%d PAWCS", idx++);
	//PrintMessage("%d TwoPoints", idx++);
	//PrintMessage("%d ViBe", idx++);
	//PrintMessage("%d CodeBook", idx++);

	int type = GetTGMTConfig()->ReadValueInt("background_subtraction_type") - 1;
	if (type == -1)
		type = 0;

	GetTGMTbgs()->Init((TGMTbgs::BgsType)type );

	std::string video = GetTGMTConfig()->ReadValueString("video", "");
	if (TGMTfile::IsVideo(video))
	{		
		m_frameAmount = GetTGMTvideo()->GetAmountFrame(video);
		GetTGMTvideo()->OnNewFrame = OnVideoFrameBgs;

		//START_COUNT_TIME("BGS");
		GetTGMTvideo()->PlayVideo(video);
	}
	else
	{
		int cameraID = GetTGMTConfig()->ReadValueInt("camera_id");
		GetTGMTcamera()->OnNewFrame = OnVideoFrameBgs;

		GetTGMTcamera()->PlayCamera(cameraID);
	}

}