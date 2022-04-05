"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.deactivate = exports.activate = void 0;
// The module 'vscode' contains the VS Code extensibility API
// Import the module and reference it with the alias vscode in your code below
const vscode_1 = require("vscode");
const CodelensProvider_1 = require("./CodelensProvider");
const CodeCompletionProvider_1 = require("./CodeCompletionProvider");
// this method is called when your extension is activated
// your extension is activated the very first time the command is executed
let disposables = [];
function activate(context) {
    const codelensProvider = new CodelensProvider_1.CodelensProvider();
    const codeCompletionProvider = new CodeCompletionProvider_1.CodeCompletionItemProvider();
    const provider1 = vscode_1.languages.registerCodeLensProvider("csharp", codelensProvider);
    const provider2 = vscode_1.languages.registerCompletionItemProvider("csharp", codeCompletionProvider, '@');
    vscode_1.commands.registerCommand("polychrome.enableCodeLens", () => {
        vscode_1.workspace.getConfiguration("polychromee").update("enableCodeLens", true, true);
    });
    vscode_1.commands.registerCommand("polychrome.disableCodeLens", () => {
        vscode_1.workspace.getConfiguration("polychrome").update("enableCodeLens", false, true);
    });
    vscode_1.commands.registerCommand("polychrome.codelensAction", (args) => {
    });
    context.subscriptions.push(provider1, provider2);
}
exports.activate = activate;
// this method is called when your extension is deactivated
function deactivate() {
    if (disposables) {
        disposables.forEach(item => item.dispose());
    }
    disposables = [];
}
exports.deactivate = deactivate;
//# sourceMappingURL=extension.js.map