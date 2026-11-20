.class Lcom/racas/black/BundleActivity$1$1;
.super Ljava/lang/Object;
.source "BundleActivity.java"

# interfaces
.implements Lcom/racas/black/BundleFiles$Status;


# annotations
.annotation system Ldalvik/annotation/EnclosingMethod;
    value = Lcom/racas/black/BundleActivity$1;->run()V
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$1:Lcom/racas/black/BundleActivity$1;


# direct methods
.method constructor <init>(Lcom/racas/black/BundleActivity$1;)V
    .locals 0
    .annotation system Ldalvik/annotation/MethodParameters;
        accessFlags = {
            0x8010
        }
        names = {
            null
        }
    .end annotation

    .line 44
    iput-object p1, p0, Lcom/racas/black/BundleActivity$1$1;->this$1:Lcom/racas/black/BundleActivity$1;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method synthetic lambda$update$0$com-racas-black-BundleActivity$1$1(JJLjava/lang/String;)V
    .locals 2

    .line 47
    iget-object v0, p0, Lcom/racas/black/BundleActivity$1$1;->this$1:Lcom/racas/black/BundleActivity$1;

    iget-object v0, v0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {v0}, Lcom/racas/black/BundleActivity;->access$000(Lcom/racas/black/BundleActivity;)Z

    move-result v0

    if-nez v0, :cond_0

    const-wide/16 v0, 0x3e8

    mul-long/2addr p1, v0

    const-wide/16 v0, 0x1

    invoke-static {v0, v1, p3, p4}, Ljava/lang/Math;->max(JJ)J

    move-result-wide p3

    div-long/2addr p1, p3

    long-to-int p1, p1

    iget-object p2, p0, Lcom/racas/black/BundleActivity$1$1;->this$1:Lcom/racas/black/BundleActivity$1;

    iget-object p2, p2, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {p2}, Lcom/racas/black/BundleActivity;->access$300(Lcom/racas/black/BundleActivity;)Landroid/widget/ProgressBar;

    move-result-object p2

    invoke-virtual {p2, p1}, Landroid/widget/ProgressBar;->setProgress(I)V

    iget-object p2, p0, Lcom/racas/black/BundleActivity$1$1;->this$1:Lcom/racas/black/BundleActivity$1;

    iget-object p2, p2, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {p2}, Lcom/racas/black/BundleActivity;->access$400(Lcom/racas/black/BundleActivity;)Landroid/widget/TextView;

    move-result-object p2

    new-instance p3, Ljava/lang/StringBuilder;

    invoke-direct {p3}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual {p3, p5}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p3

    const-string p4, "\n"

    invoke-virtual {p3, p4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p3

    sget-object p4, Ljava/util/Locale;->ROOT:Ljava/util/Locale;

    div-int/lit8 p1, p1, 0xa

    invoke-static {p1}, Ljava/lang/Integer;->valueOf(I)Ljava/lang/Integer;

    move-result-object p1

    filled-new-array {p1}, [Ljava/lang/Object;

    move-result-object p1

    const-string p5, "%d%%"

    invoke-static {p4, p5, p1}, Ljava/lang/String;->format(Ljava/util/Locale;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;

    move-result-object p1

    invoke-virtual {p3, p1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p1

    invoke-virtual {p2, p1}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    :cond_0
    return-void
.end method

.method public update(Ljava/lang/String;JJ)V
    .locals 9
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    .line 45
    iget-object v0, p0, Lcom/racas/black/BundleActivity$1$1;->this$1:Lcom/racas/black/BundleActivity$1;

    iget-object v0, v0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {v0}, Lcom/racas/black/BundleActivity;->access$000(Lcom/racas/black/BundleActivity;)Z

    move-result v0

    if-nez v0, :cond_1

    .line 46
    invoke-static {}, Ljava/lang/System;->currentTimeMillis()J

    move-result-wide v0

    iget-object v2, p0, Lcom/racas/black/BundleActivity$1$1;->this$1:Lcom/racas/black/BundleActivity$1;

    iget-object v2, v2, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {v2}, Lcom/racas/black/BundleActivity;->access$100(Lcom/racas/black/BundleActivity;)J

    move-result-wide v2

    sub-long v2, v0, v2

    const-wide/16 v4, 0xfa

    cmp-long v2, v2, v4

    if-gez v2, :cond_0

    cmp-long v2, p2, p4

    if-eqz v2, :cond_0

    return-void

    :cond_0
    iget-object v2, p0, Lcom/racas/black/BundleActivity$1$1;->this$1:Lcom/racas/black/BundleActivity$1;

    iget-object v2, v2, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {v2, v0, v1}, Lcom/racas/black/BundleActivity;->access$102(Lcom/racas/black/BundleActivity;J)J

    .line 47
    iget-object v0, p0, Lcom/racas/black/BundleActivity$1$1;->this$1:Lcom/racas/black/BundleActivity$1;

    iget-object v0, v0, Lcom/racas/black/BundleActivity$1;->this$0:Lcom/racas/black/BundleActivity;

    invoke-static {v0}, Lcom/racas/black/BundleActivity;->access$200(Lcom/racas/black/BundleActivity;)Landroid/os/Handler;

    move-result-object v0

    new-instance v8, Lcom/racas/black/BundleActivity$1$1$$ExternalSyntheticLambda0;

    move-object v1, v8

    move-object v2, p0

    move-wide v3, p2

    move-wide v5, p4

    move-object v7, p1

    invoke-direct/range {v1 .. v7}, Lcom/racas/black/BundleActivity$1$1$$ExternalSyntheticLambda0;-><init>(Lcom/racas/black/BundleActivity$1$1;JJLjava/lang/String;)V

    invoke-virtual {v0, v8}, Landroid/os/Handler;->post(Ljava/lang/Runnable;)Z

    .line 48
    return-void

    .line 45
    :cond_1
    new-instance p1, Ljava/io/IOException;

    const-string p2, "\u062a\u0648\u0642\u0641 \u0627\u0644\u062a\u062c\u0647\u064a\u0632\u061b \u064a\u0645\u0643\u0646\u0643 \u0641\u062a\u062d \u0627\u0644\u062a\u0637\u0628\u064a\u0642 \u0645\u062c\u062f\u062f\u0627\u064b"

    invoke-direct {p1, p2}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw p1
.end method
