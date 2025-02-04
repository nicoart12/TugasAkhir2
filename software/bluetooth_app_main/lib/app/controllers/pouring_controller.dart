import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'command_controller.dart';
import 'recipe_controller.dart';
import '../views/add_command_view.dart';

class PouringDialogController extends GetxController {
  static void onCancelPressed(BuildContext context) {
    RecipeController().onNewCommandButtonPressed();
    Navigator.pop(context);
  }

  static void onNextPressed(BuildContext context) {
    if (CommandController.isEditCommand.isTrue) {
      Navigator.pop(context);
      CommandController.isEditCommand.value = false;
    } else {
      CommandController.validateCommandInput();
      CommandController.saveNewCommand();
      debugPrint(
          'Command current step: ${CommandController.currentStep.value}');
      debugPrint(
          'Command num step: ${CommandController.commandnumStepCtrl.text}');

      if (CommandController.isInputCommandValid.isFalse) {
        debugPrint('Command input is not valid.');
        return;
      }
      int commandNumStep = int.parse(CommandController.commandnumStepCtrl.text);

      if (CommandController.currentStep.value < commandNumStep) {
        RecipeController().onNewCommandButtonPressed();
        debugPrint(
            '[onnextpressed] current recipe length: ${RecipeController.currentRecipe!.commandList.length}');
        CommandView.showPouringDialog(context);
        Navigator.pop(context);
      } else if (CommandController.currentStep.value == commandNumStep) {
        RecipeController().onNewCommandButtonPressed();
        Navigator.pop(context);
        Navigator.pop(context);
        CommandController.commandnumStepCtrl.clear();
        CommandController.resetSteps();
      } else {
        CommandController.resetSteps();
        Navigator.pop(context);
      }
    }
  }
}
