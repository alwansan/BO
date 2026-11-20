.class Lcom/racas/black/CinemaActivity$13;
.super Ljava/lang/Object;
.source "CinemaActivity.java"

# interfaces
.implements Ljava/lang/Runnable;


# annotations
.annotation system Ldalvik/annotation/EnclosingClass;
    value = Lcom/racas/black/CinemaActivity;
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

    .line 137
    iput-object p1, p0, Lcom/racas/black/CinemaActivity$13;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method public run()V
    .locals 3

    .line 138
    iget-object v0, p0, Lcom/racas/black/CinemaActivity$13;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object v0

    if-nez v0, :cond_0

    return-void

    .line 139
    :cond_0
    iget-object v0, p0, Lcom/racas/black/CinemaActivity$13;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$600(Lcom/racas/black/CinemaActivity;)Z

    move-result v0

    if-nez v0, :cond_1

    iget-object v0, p0, Lcom/racas/black/CinemaActivity$13;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$800(Lcom/racas/black/CinemaActivity;)Landroid/widget/SeekBar;

    move-result-object v0

    iget-object v1, p0, Lcom/racas/black/CinemaActivity$13;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v1}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object v1

    invoke-virtual {v1}, Landroid/widget/VideoView;->getCurrentPosition()I

    move-result v1

    invoke-virtual {v0, v1}, Landroid/widget/SeekBar;->setProgress(I)V

    .line 140
    :cond_1
    iget-object v0, p0, Lcom/racas/black/CinemaActivity$13;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$1200(Lcom/racas/black/CinemaActivity;)Landroid/widget/TextView;

    move-result-object v0

    new-instance v1, Ljava/lang/StringBuilder;

    invoke-direct {v1}, Ljava/lang/StringBuilder;-><init>()V

    iget-object v2, p0, Lcom/racas/black/CinemaActivity$13;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v2}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object v2

    invoke-virtual {v2}, Landroid/widget/VideoView;->getCurrentPosition()I

    move-result v2

    invoke-static {v2}, Lcom/racas/black/CinemaActivity;->access$1100(I)Ljava/lang/String;

    move-result-object v2

    invoke-virtual {v1, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v1

    const-string v2, " / "

    invoke-virtual {v1, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v1

    iget-object v2, p0, Lcom/racas/black/CinemaActivity$13;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v2}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object v2

    invoke-virtual {v2}, Landroid/widget/VideoView;->getDuration()I

    move-result v2

    invoke-static {v2}, Lcom/racas/black/CinemaActivity;->access$1100(I)Ljava/lang/String;

    move-result-object v2

    invoke-virtual {v1, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v1

    invoke-virtual {v1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v1

    invoke-virtual {v0, v1}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    .line 141
    iget-object v0, p0, Lcom/racas/black/CinemaActivity$13;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$1000(Lcom/racas/black/CinemaActivity;)Landroid/os/Handler;

    move-result-object v0

    const-wide/16 v1, 0x1f4

    invoke-virtual {v0, p0, v1, v2}, Landroid/os/Handler;->postDelayed(Ljava/lang/Runnable;J)Z

    .line 142
    return-void
.end method
