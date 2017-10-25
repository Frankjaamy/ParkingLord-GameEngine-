#pragma once
#include "ActorController.h"
#include "ViehcleController.h"
#include "DefaultController.h"
namespace Engine {

	struct ControllerInfo {
		struct AdditionalInfo {
			float data_1_;
			float data_2_;
			AdditionalInfo(float * data) :data_1_(data[0]), data_2_(data[1]) {

			}
			AdditionalInfo() :data_1_(0), data_2_(0) {

			}
		};

		const char * controller_name_;
		AdditionalInfo controller_info;
		ControllerInfo(const char * controller_name, float * data) :controller_info(data) {
			controller_name_ = controller_name;
		}
		ControllerInfo() :controller_info() {
			controller_name_ = "DefaultController";
		}
	};


	class ControllerFactory {
	public:
		static ActorController * CreateController(const ControllerInfo & info);
		static bool RegisterController(ActorController * controller, void * bind_object);
	};
}



