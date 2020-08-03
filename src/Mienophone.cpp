#include "plugin.hpp"


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

	void process(const ProcessArgs& args) override {
		float anger = 1.2;
		float contempt = 5.7;
		float disgust = 0.9;
		float fear = 3.2;
		float happyness = 10.0;
		float neutral = 5.0;
		float sadness = 6.1;
		float surprise = 3.4;

		outputs[ANGER_OUTPUT].setVoltage(anger);
		outputs[CONTEMPT_OUTPUT].setVoltage(contempt);
		outputs[DISGUST_OUTPUT].setVoltage(disgust);
		outputs[FEAR_OUTPUT].setVoltage(fear);
		outputs[HAPPYNESS_OUTPUT].setVoltage(happyness);
		outputs[NEUTRAL_OUTPUT].setVoltage(neutral);
		outputs[SADNESS_OUTPUT].setVoltage(sadness);
		outputs[SURPRISE_OUTPUT].setVoltage(surprise);
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

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.71, 54.135)), module, Mienophone::ANGER_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.71, 94.325)), module, Mienophone::CONTEMPT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.71, 134.515)), module, Mienophone::DISGUST_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.71, 174.755)), module, Mienophone::FEAR_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.71, 214.895)), module, Mienophone::HAPPYNESS_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.71, 255.085)), module, Mienophone::NEUTRAL_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.71, 295.26)), module, Mienophone::SADNESS_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.71, 335.465)), module, Mienophone::SURPRISE_OUTPUT));
	}
};


Model* modelMienophone = createModel<Mienophone, MienophoneWidget>("Mienophone");
