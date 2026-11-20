.class Lcom/racas/black/CinemaActivity$5;
.super Ljava/lang/Object;
.source "CinemaActivity.java"

# interfaces
.implements Landroid/view/View$OnClickListener;


# annotations
.annotation system Ldalvik/annotation/EnclosingMethod;
    value = Lcom/racas/black/CinemaActivity;->startMovie(II)V
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$0:Lcom/racas/black/CinemaActivity;


# direct methods
.method constructor <init>(Lcom/racas/black/CinemaActivity;)V
    .locals 0
    .annotation system Ldalvik/annotation/MethodParameters;
        accessFlags = {
            0x8010
        }
        names = {
            null
        }
    .end annotation

    .line 99
    iput-object p1, p0, Lcom/racas/black/CinemaActivity$5;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method public onClick(Landroid/view/View;)V
    .locals 1

    .line 100
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$5;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object p1

    if-nez p1, :cond_0

    return-void

    .line 101
    :cond_0
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$5;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object p1

    invoke-virtual {p1}, Landroid/widget/VideoView;->isPlaying()Z

    move-result p1

    if-eqz p1, :cond_1

    iget-object p1, p0, Lcom/racas/black/CinemaActivity$5;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object p1

    invoke-virtual {p1}, Landroid/widget/VideoView;->pause()V

    iget-object p1, p0, Lcom/racas/black/CinemaActivity$5;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$300(Lcom/racas/black/CinemaActivity;)Landroid/widget/Button;

    move-result-object p1

    const-string v0, "\u062a\u0634\u063a\u064a\u0644"

    invoke-virtual {p1, v0}, Landroid/widget/Button;->setText(Ljava/lang/CharSequence;)V

    goto :goto_0

    .line 102
    :cond_1
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$5;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object p1

    invoke-virtual {p1}, Landroid/widget/VideoView;->start()V

    iget-object p1, p0, Lcom/racas/black/CinemaActivity$5;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$300(Lcom/racas/black/CinemaActivity;)Landroid/widget/Button;

    move-result-object p1

    const-string v0, "\u0625\u064a\u0642\u0627\u0641 \u0645\u0624\u0642\u062a"

    invoke-virtual {p1, v0}, Landroid/widget/Button;->setText(Ljava/lang/CharSequence;)V

    .line 103
    :goto_0
    return-void
.end method
