// The module 'vscode' contains the VS Code extensibility API
// Import the module and reference it with the alias vscode in your code below
import { ExtensionContext, languages, commands, Disposable, workspace, window, TreeItem } from 'vscode';
import { CodelensProvider } from './CodelensProvider';
import { CodeCompletionItemProvider } from './CodeCompletionProvider';

// this method is called when your extension is activated
// your extension is activated the very first time the command is executed

let disposables: Disposable[] = [];

export function activate(context: ExtensionContext) {
    const codelensProvider = new CodelensProvider();
	const codeCompletionProvider = new CodeCompletionItemProvider();
    const provider1 = languages.registerCodeLensProvider("csharp", codelensProvider);
	const provider2 = languages.registerCompletionItemProvider("csharp", codeCompletionProvider, '@');

    commands.registerCommand("polychrome.enableCodeLens", () => {
        workspace.getConfiguration("polychromee").update("enableCodeLens", true, true);
    });

    commands.registerCommand("polychrome.disableCodeLens", () => {
        workspace.getConfiguration("polychrome").update("enableCodeLens", false, true);
    });

    commands.registerCommand("polychrome.codelensAction", (args: any) => {
        
    });

	context.subscriptions.push(provider1, provider2);
}

// this method is called when your extension is deactivated
export function deactivate() {
    if (disposables) {
        disposables.forEach(item => item.dispose());
    }
    disposables = [];
}