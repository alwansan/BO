.class Lcom/racas/black/BundleActivity$1;
.super Ljava/lang/Object;
.source "BundleActivity.java"

# interfaces
.implements Ljava/lang/Runnable;


# annotations
.annotation system Ldalvik/annotation/EnclosingMethod;
    value = Lcom/racas/black/BundleActivity;->begin(Landroid/widget/Button;)V
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$0:Lcom/racas/black/BundleActivity;

.field final synthetic val$retry:Landroid/widget/Button;


# direct methods
.method constructor <init>(Lcom/racas/black/BundleActivity;Landroid/widget/Button;)V
    .locals 0
    .annotation system Ldalvik/annotation/MethodParameters;
        accessFlags = {
            0x8010,
            0x1010
        }
        names = {
            null,
            null
        }
    .end annotation

    .annotation system Ldalvik/annotation/Signature;
        value = {
            "()V"
        }
    .end annotation

    .line 40
    iput-object p1, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    iput-object p2, p0, Lcom/racas/black/BundleActivity$1;->val$retry:Landroid/widget/Button;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method synthetic lambda$run$0$com-racas-black-BundleActivity$1()Ljava/io/InputStream;
    .locals 2
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    .line 44
    iget-object v0, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-virtual {v0}, Lcom/racas/black/BundleActivity;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v0

    const-string v1, "bundle/game.iso"

    invoke-virtual {v0, v1}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;

    move-result-object v0

    return-object v0
.end method

.method synthetic lambda$run$1$com-racas-black-BundleActivity$1()V
    .locals 2

    .line 49
    iget-object v0, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    const/4 v1, 0x0

    invoke-static {v0, v1}, Lcom/racas/black/BundleActivity;->access$502(Lcom/racas/black/BundleActivity;Z)Z

    iget-object v0, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    const/4 v1, 0x1

    invoke-static {v0, v1}, Lcom/racas/black/BundleActivity;->access$602(Lcom/racas/black/BundleActivity;Z)Z

    iget-object v0, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {v0}, Lcom/racas/black/BundleActivity;->access$700(Lcom/racas/black/BundleActivity;)V

    return-void
.end method

.method synthetic lambda$run$2$com-racas-black-BundleActivity$1(Ljava/lang/Exception;Landroid/widget/Button;)V
    .locals 4

    .line 51
    iget-object v0, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    const/4 v1, 0x0

    invoke-static {v0, v1}, Lcom/racas/black/BundleActivity;->access$502(Lcom/racas/black/BundleActivity;Z)Z

    iget-object v0, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {v0}, Lcom/racas/black/BundleActivity;->access$000(Lcom/racas/black/BundleActivity;)Z

    move-result v0

    if-nez v0, :cond_0

    iget-object v0, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {v0}, Lcom/racas/black/BundleActivity;->access$400(Lcom/racas/black/BundleActivity;)Landroid/widget/TextView;

    move-result-object v0

    new-instance v2, Ljava/lang/StringBuilder;

    invoke-direct {v2}, Ljava/lang/StringBuilder;-><init>()V

    const-string v3, "\u062a\u0639\u0630\u0631 \u062a\u062c\u0647\u064a\u0632 \u0627\u0644\u0644\u0639\u0628\u0629\n"

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {p1}, Ljava/lang/Exception;->getMessage()Ljava/lang/String;

    move-result-object p1

    invoke-virtual {v2, p1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    const-string v2, "\n\u0644\u0645 \u062a\u062a\u063a\u064a\u0631 \u0645\u0644\u0641\u0627\u062a \u0627\u0644\u062d\u0641\u0638 \u0648\u0627\u0644\u0625\u0639\u062f\u0627\u062f\u0627\u062a."

    invoke-virtual {p1, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p1

    invoke-virtual {v0, p1}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    iget-object p1, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {p1}, Lcom/racas/black/BundleActivity;->access$300(Lcom/racas/black/BundleActivity;)Landroid/widget/ProgressBar;

    move-result-object p1

    const/16 v0, 0x8

    invoke-virtual {p1, v0}, Landroid/widget/ProgressBar;->setVisibility(I)V

    invoke-virtual {p2, v1}, Landroid/widget/Button;->setVisibility(I)V

    :cond_0
    return-void
.end method

.method public run()V
    .locals 9

    .line 42
    :try_start_0
    iget-object v0, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    const/4 v1, 0x0

    invoke-virtual {v0, v1}, Lcom/racas/black/BundleActivity;->getExternalFilesDir(Ljava/lang/String;)Ljava/io/File;

    move-result-object v0

    if-eqz v0, :cond_0

    .line 43
    new-instance v1, Ljava/io/File;

    const-string v2, "assets"

    invoke-direct {v1, v0, v2}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    new-instance v2, Ljava/io/File;

    iget-object v0, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-virtual {v0}, Lcom/racas/black/BundleActivity;->getFilesDir()Ljava/io/File;

    move-result-object v0

    const-string v3, "bundle-v08.marker"

    invoke-direct {v2, v0, v3}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    const-string v3, "BLACK-Arabic-MohammePS4-v0.8.iso"

    const-string v6, "f6d15ff277eb19ef4fb9092e245599c7ae68f5a49c6ac7db60934a32ec246c62"

    new-instance v7, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda0;

    invoke-direct {v7, p0}, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda0;-><init>(Lcom/racas/black/BundleActivity$1;)V

    new-instance v8, Lcom/racas/black/BundleActivity$1$1;

    invoke-direct {v8, p0}, Lcom/racas/black/BundleActivity$1$1;-><init>(Lcom/racas/black/BundleActivity$1;)V

    const-wide/32 v4, 0x6ae1c000

    invoke-static/range {v1 .. v8}, Lcom/racas/black/BundleFiles;->prepare(Ljava/io/File;Ljava/io/File;Ljava/lang/String;JLjava/lang/String;Lcom/racas/black/BundleFiles$Source;Lcom/racas/black/BundleFiles$Status;)Ljava/io/File;

    .line 49
    iget-object v0, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {v0}, Lcom/racas/black/BundleActivity;->access$200(Lcom/racas/black/BundleActivity;)Landroid/os/Handler;

    move-result-object v0

    new-instance v1, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda1;

    invoke-direct {v1, p0}, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda1;-><init>(Lcom/racas/black/BundleActivity$1;)V

    invoke-virtual {v0, v1}, Landroid/os/Handler;->post(Ljava/lang/Runnable;)Z

    .line 52
    goto :goto_0

    .line 42
    :cond_0
    new-instance v0, Ljava/io/IOException;

    const-string v1, "\u0630\u0627\u0643\u0631\u0629 \u0627\u0644\u0647\u0627\u062a\u0641 \u063a\u064a\u0631 \u0645\u062a\u0627\u062d\u0629 \u062d\u0627\u0644\u064a\u0627\u064b"

    invoke-direct {v0, v1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw v0
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    .line 50
    :catch_0
    move-exception v0

    .line 51
    iget-object v1, p0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {v1}, Lcom/racas/black/BundleActivity;->access$200(Lcom/racas/black/BundleActivity;)Landroid/os/Handler;

    move-result-object v1

    iget-object v2, p0, Lcom/racas/black/BundleActivity$1;->val$retry:Landroid/widget/Button;

    new-instance v3, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda2;

    invoke-direct {v3, p0, v0, v2}, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda2;-><init>(Lcom/racas/black/BundleActivity$1;Ljava/lang/Exception;Landroid/widget/Button;)V

    invoke-virtual {v1, v3}, Landroid/os/Handler;->post(Ljava/lang/Runnable;)Z

    .line 53
    :goto_0
    return-void
.end method
