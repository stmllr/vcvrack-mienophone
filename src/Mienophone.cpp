#include "plugin.hpp"

#include <stdio.h>
#include <curl/curl.h>

#include <stdlib.h>
#include <string.h>

#define FACE_API "http://localhost:8080/face/v1.0/detect"
#define BUFFER_SIZE (256 * 1024) /* 256 KB */
#define BPM 120

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
		HAPPINESS_OUTPUT,
		NEUTRAL_OUTPUT,
		SADNESS_OUTPUT,
		SURPRISE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ANGER_LIGHT,
		CONTEMPT_LIGHT,
		DISGUST_LIGHT,
		FEAR_LIGHT,
		HAPPINESS_LIGHT,
		NEUTRAL_LIGHT,
		SADNESS_LIGHT,
		SURPRISE_LIGHT,
		NUM_LIGHTS
	};

	enum Emotions {
		ANGER,
		CONTEMPT,
		DISGUST,
		FEAR,
		HAPPINESS,
		NEUTRAL,
		SADNESS,
		SURPRISE,
		NUMS
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

	unsigned long counter = 0;
	float emotions[8] = {0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000};

	void process(const ProcessArgs& args) override {
		// BPM 120
		// BPM / 60 = 2
		// args.sampleRate / BPM/2
		if (counter++ % (long) (args.sampleRate / (BPM / 60)) == 0) {
			setEmotions(emotions);
		}

		for (int i = 0; i < NUM_OUTPUTS; i++) {
			outputs[i].setVoltage(emotions[i]);
		}

		for (int i = 0; i < NUM_LIGHTS; i++) {
			if (outputs[i].isConnected()) {
				lights[i].setBrightness(outputs[i].getVoltage());
			} else {
				lights[i].setBrightness(0.0);
			}
		}
	}

	void setEmotions(float emotions[]) {
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
			json_t *root = json_loads(chunk.memory, flags, &error);
			json_t *obj;
			const char *key;

			// [{"faceAttributes":{"emotion":{"anger":0.986, ... }}}]
			if (json_is_array(root)) {
				json_t *face = json_array_get(root, 0);
				if (json_is_object(face)) {
					json_t *faceAttributes = json_object_get(face, "faceAttributes");
					if (json_is_object(faceAttributes)) {
						json_t *faceEmotion = json_object_get(faceAttributes, "emotion");
						if (json_is_object(faceEmotion)) {
							json_object_foreach(faceEmotion, key, obj){
								double value = json_number_value(obj);
								value = value * 10;
								printf("%s => %lf\n", key, value);

								if(!strcmp(key, "anger")) {
									emotions[Mienophone::ANGER] = (float) value;
								}
								if(!strcmp(key, "contempt")) {
									emotions[Mienophone::CONTEMPT] = (float) value;
								}
								if(!strcmp(key, "disgust")) {
									emotions[Mienophone::DISGUST] = (float) value;
								}
								if(!strcmp(key, "fear")) {
									emotions[Mienophone::FEAR] = (float) value;
								}
								if(!strcmp(key, "happiness")) {
									emotions[Mienophone::HAPPINESS] = (float) value;
								}
								if(!strcmp(key, "neutral")) {
									emotions[Mienophone::NEUTRAL] = (float) value;
								}
								if(!strcmp(key, "sadness")) {
									emotions[Mienophone::SADNESS] = (float) value;
								}
								if(!strcmp(key, "surprise")) {
									emotions[Mienophone::SURPRISE] = (float) value;
								}
							}
						}
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

		float outputX = 10.153;
		double outputY[] = {21.0, 35.1, 49.2, 63.3, 77.4, 91.5, 105.6, 119.7};

		float lightX = 9;
		double lightY[] = {20.0, 34.1, 48.2, 62.3, 76.4, 90.5,104.6, 118.7};

		for (int output = 0; output < Mienophone::NUM_OUTPUTS; output++) {
			addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(outputX, outputY[output])), module, output));
			addChild(createLight<SmallLight<GreenLight>>(mm2px(Vec(lightX, lightY[output])), module, output));

		}
	}
};

Model* modelMienophone = createModel<Mienophone, MienophoneWidget>("Mienophone");
