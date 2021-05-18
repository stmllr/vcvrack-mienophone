#include "plugin.hpp"

#include <stdio.h>
#include <curl/curl.h>

#include <stdlib.h>
#include <string.h>

#define FACE_API "http://localhost:8080/face/v1.0/detect"
#define BUFFER_SIZE (256 * 1024) /* 256 KB */
#define SAMPLE_RATE 44100

struct Mienophone : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		ANGER_OUTPUT,
		CONTEMPT_OUTPUT,
		DISGUST_OUTPUT,
		FEAR_OUTPUT,
		HAPPYNESS_OUTPUT,
		NEUTRAL_OUTPUT,
		SADNESS_OUTPUT,
		SURPRISE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Mienophone() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	struct MemoryStruct {
		char *memory;
		size_t size;
	};

	static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
		size_t realsize = size * nmemb;
		struct MemoryStruct *mem = (struct MemoryStruct *)userp;

		char *ptr = (char *) realloc(mem->memory, mem->size + realsize + 1);
		if(!ptr) {
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
		}

		mem->memory = ptr;
		memcpy(&(mem->memory[mem->size]), contents, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;

		return realsize;
	}

	struct emotions {
		// 0.0 to 10.0
		float anger;
		float contempt;
		float disgust;
		float fear;
		float happyness;
		float neutral;
		float sadness;
		float surprise;
	};

	emotions currentEmotions;
	unsigned long counter = 0;

	void process(const ProcessArgs& args) override {

		if (counter++ % SAMPLE_RATE == 0) {
			setEmotion(&currentEmotions);
		}

		outputs[ANGER_OUTPUT].setVoltage(currentEmotions.anger);
		outputs[CONTEMPT_OUTPUT].setVoltage(currentEmotions.contempt);
		outputs[DISGUST_OUTPUT].setVoltage(currentEmotions.disgust);
		outputs[FEAR_OUTPUT].setVoltage(currentEmotions.fear);
		outputs[HAPPYNESS_OUTPUT].setVoltage(currentEmotions.happyness);
		outputs[NEUTRAL_OUTPUT].setVoltage(currentEmotions.neutral);
		outputs[SADNESS_OUTPUT].setVoltage(currentEmotions.sadness);
		outputs[SURPRISE_OUTPUT].setVoltage(currentEmotions.surprise);
	}

	void setEmotion(emotions *currentEmotions) {
		CURL *curl_handle;
		CURLcode res;

		struct MemoryStruct chunk;

		chunk.memory = (char *) malloc(1);  /* will be grown as needed by the realloc above */
		chunk.size = 0;    /* no data at this point */

		curl_handle = curl_easy_init();
		curl_easy_setopt(curl_handle, CURLOPT_URL, FACE_API);
		/* Tell libcurl to follow redirection */
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

		/* set our custom set of headers */
		//struct curl_slist *curl_headers_chunk = NULL;
		//curl_headers_chunk = curl_slist_append(curl_headers_chunk, "Content-Type: application/octet-stream");
		//curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, curl_headers_chunk);

		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "vcv-mienophone/1.0");

		/* send all data to this function  */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl_handle);
		/* Check for errors */
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else {
			json_error_t error;
			size_t flags = 0;
			json_t *json = json_loads(chunk.memory, flags, &error);
			json_t *obj;
			const char *key;

			if (json_is_object(json)) {
				json_object_foreach(json, key, obj){
					double value = json_number_value(obj);
					value = value * 10;
					printf("%s => %lf\n", key, value);

					if(!strcmp(key, "anger")) {
						currentEmotions->anger = (float) value;
					}
					if(!strcmp(key, "contempt")) {
						currentEmotions->contempt = (float) value;
					}
					if(!strcmp(key, "disgust")) {
						currentEmotions->disgust = (float) value;
					}
					if(!strcmp(key, "fear")) {
						currentEmotions->fear = (float) value;
					}
					if(!strcmp(key, "happyness")) {
						currentEmotions->happyness = (float) value;
					}
					if(!strcmp(key, "neutral")) {
						currentEmotions->neutral = (float) value;
					}
					if(!strcmp(key, "sadness")) {
						currentEmotions->sadness = (float) value;
					}
					if(!strcmp(key, "surprise")) {
						currentEmotions->surprise = (float) value;
					}
				}
			}
		}

		/* always cleanup */
		curl_easy_cleanup(curl_handle);
		free(chunk.memory);
	}

};

struct MienophoneWidget : ModuleWidget {
	MienophoneWidget(Mienophone* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Mienophone.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.153, 20.667)), module, Mienophone::ANGER_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.153, 35.483)), module, Mienophone::CONTEMPT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.153, 49.639)), module, Mienophone::DISGUST_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.153, 62.994)), module, Mienophone::FEAR_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.153, 77.684)), module, Mienophone::HAPPYNESS_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.153, 92.651)), module, Mienophone::NEUTRAL_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.153, 106.015)), module, Mienophone::SADNESS_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.153, 119.637)), module, Mienophone::SURPRISE_OUTPUT));
	}
};


Model* modelMienophone = createModel<Mienophone, MienophoneWidget>("Mienophone");
